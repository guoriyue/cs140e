// skeleton for nrf driver.   
//    - sorry it's so large.  the device is pretty complicated and we're
//      just doing a single lab.
//
// --------------------------------------------------------------------
// key files:
//  1. nrf.h: <nrf_t> the nic structure and <nrf_config_t> some of the
//     configuration.  
//  2. <nrf-hw-support.h>: many accessors for reading and writing
//     NRF registers and for SPI.  you really want to look at this 
//     to save time.
//  3. <nrf-hw-support.c>: SPI read/write routines, routines to dump the 
//     NRF configuration.
//  4. <nrf-default-values.h> default values for NRF hardware choices.
//     you want to look at to see what we can change and for where
//     to get the requested values.
//  5. <nrf-public.c> simple veneer over the top of NRF interface.
//
// --------------------------------------------------------------------
//  big picture: stay in RX by default.  only go TX to send then back.
//  0. at startup go PowerDown->StandbyI->RX
//  1. stay in RX p22]
//  2. to send: RX -> StandbyI ->TX -> Standby-II [can send multiple]
//      then StandbyI->RX mode to get back into RX.
//
// alot is in comments, but not everything.  if you're going to use the
// comment, should double check that it's accurate!
//
// --------------------------------------------------------------------
// you definitely want to to:
//  1. check anything you can to make sure the state matches what you think
//     it should be
//  2. always follow the state machine in the datasheet.
//  3. general strategy: in RX mode, quickly flip to TX if have something
//     to send, flip back to RX.
//
// --------------------------------------------------------------------
// useful extensions:
// 0. send large packets by fragmenting and reassembling.
// 1. a reliable FIFO that handles duplicates, out of order, corruption,
//    packets from previous sessions (e.g., when one reboots).
// 2. network boot.  this is fun, very useful.
// 3. possible: add interrupts for receiving packets.  don't do much
//    in the handler.  can also let the remote node send "REBOOT"
//    commands so that you can control a big network.
//
#include "nrf.h"
#include "nrf-hw-support.h"

// precompute the rx and tx configs so we can 
// just set them without thinking.
//
// enable crc, enable 2 byte
#   define set_bit(x)       (1<<(x))
#   define enable_crc      set_bit(3)
#   define crc_two_byte    set_bit(2)
#   define pwr_up          set_bit(PWR_UP_BIT)
#   define mask_int        set_bit(6)|set_bit(5)|set_bit(4)
enum {
    // pre-computed: can write into NRF_CONFIG to enable TX.
    tx_config = enable_crc | crc_two_byte | pwr_up | mask_int,
    // pre-computed: can write into NRF_CONFIG to enable RX.
    rx_config = tx_config  | set_bit(PRIM_RX)
} ;

// seperate out so we can do some error checking.
static inline void ce_lo(uint8_t ce) {
    if(ce != 6 && ce != 5)
        panic("expected 6 or 5, have: %d\n", ce);
    dev_barrier();
    // todo("implement this\n");
    gpio_write(ce, 0);
}

// seperate out so we can do some error checking.
static inline void ce_hi(uint8_t ce) {
    if(ce != 6 && ce != 5)
        panic("expected 6 or 5, have: %d\n", ce);
    dev_barrier();
    // todo("implement this\n");
    gpio_write(ce, 1);
}

// put the device in RX mode.
static void nrf_rx_mode(nrf_t *n) {
    // todo("go to RX with delay");

    nrf_set_pwrup_on(n);
    delay_ms(100);

    nrf_put8_chk(n, NRF_CONFIG, rx_config);
    ce_hi(n->config.ce_pin);
    delay_us(140); // 10 + 130

    assert(nrf_is_rx(n));

    nrf_opt_assert(n, nrf_is_rx(n));
}

// go from RX from TX via the state diagram.
// instead eliminate the 10usec requirement by going from:
//       RX -> StandbyI (by setting CE=0) 
//          -> TX (by writing data into the FIFO, CE=1 for 10usec)
//          -> Standby-II (CE=1)
//          -> Standby I (CE=0)
// see the state machine on p22, and [p75]
//
// NOTE: If nRF24L01+ is in standby-II mode, it goes to standby-I mode
// immediately if CE is set low. [p75]
static void nrf_tx_mode(nrf_t *n) {
    // NOTE: tx fifo should not be empty.
    // todo("go RX->StandbyI->StandbyII->TX");

    nrf_debug("setting to tx\n");

    ce_lo(n->config.ce_pin);
    nrf_put8_chk(n, NRF_CONFIG, tx_config);
    ce_hi(n->config.ce_pin);
    delay_us(140); // 10 + 130

    nrf_opt_assert(n, nrf_is_tx(n));
}

// initialize the NRF: [extension: pass in a channel]
nrf_t *nrf_init(nrf_conf_t c, uint32_t rxaddr, unsigned acked_p) {
    // nrf_t *n = staff_nrf_init(c, rxaddr, acked_p);

    // start of initialization: go through and handle no-ack first,
    // then ack.  i'd do one test at a time.

// #if 0
    nrf_t *n = kmalloc(sizeof *n);
    n->config = c;      // set initial config.
    nrf_stat_start(n);  // reset stas.

    // configure the spi hardware and ce pin.
    n->spi = nrf_spi_init(c.ce_pin, c.spi_chip);

    n->rxaddr = rxaddr;     // set the rxaddr
    cq_init(&n->recvq, 1);  // initialize the circular queue

    // put in PWR_DOWN so can configure.
    nrf_put8_chk(n, NRF_CONFIG, 0);
    assert(!nrf_is_pwrup(n));

    // disable all eipes.
    nrf_put8_chk(n, NRF_EN_RXADDR, 0);

    if(!acked_p) {
        // reg1: disable retran.
        nrf_put8_chk(n, NRF_SETUP_RETR, 0);
        // reg2: enable pipe 1.
        // nrf_put8_chk(n, NRF_EN_RXADDR, 0b10);
        nrf_bit_set(n, NRF_EN_RXADDR, 1);
        nrf_put8_chk(n, NRF_EN_AA, 0);

        assert(nrf_pipe_is_enabled(n, 1));
        assert(!nrf_pipe_is_acked(n, 1));
        assert(!nrf_pipe_is_enabled(n, 0));
    
    } else {
        // reg=1: p57
        // both retran pipe(0) and pipe 1 have to be ENAA_P0 = 1 (p75)
        nrf_put8_chk(n, NRF_EN_AA, 0b11);

        // reg=2: p 57, enable pipes --- always enable pipe 0 for retran.
        // nrf_put8_chk(n, NRF_EN_RXADDR, 0b11);
        nrf_bit_set(n, NRF_EN_RXADDR, 0);
        nrf_bit_set(n, NRF_EN_RXADDR, 1);

        // set retrans attempt and delay
        //  - nrf_default_retran_attempts;
        //  - nrf_default_retran_delay [note you have to 
        //    convert to the right encoding]
        uint8_t retran = nrf_default_retran_attempts;
        retran = retran | (nrf_default_retran_delay << 4);
        // reg = 4: setup retran
        nrf_put8_chk(n, NRF_SETUP_RETR, retran);

        // double check
        assert(nrf_pipe_is_enabled(n, 0));
        assert(nrf_pipe_is_enabled(n, 1));
        assert(nrf_pipe_is_acked(n, 1));
        assert(nrf_pipe_is_acked(n, 0));
    }

    // turn off the other pipes.
    for(unsigned i = 2; i < 6; i++)
        nrf_bit_set(n, NRF_EN_RXADDR, i);
        // nrf_put8_chk(n, NRF_EN_RXADDR, 1<<i);
    
    // reg=3 setup address size
    nrf_put8_chk(n, NRF_SETUP_AW, nrf_default_addr_nbytes - 2);

    // clear the TX addr for determinism.
    nrf_set_addr(n, NRF_TX_ADDR, 0, nrf_default_addr_nbytes);
    // nrf_put8_chk(n, NRF_TX_ADDR, 0xE7E7E7E7E7);
    

    // Set message size = 0 for unused pipes.  
    //  [NOTE: I think redundant, but just to be sure.]
    nrf_put8_chk(n, NRF_RX_PW_P2, 0);
    nrf_put8_chk(n, NRF_RX_PW_P3, 0);
    nrf_put8_chk(n, NRF_RX_PW_P4, 0);
    nrf_put8_chk(n, NRF_RX_PW_P5, 0);

    // reg=5: RF_CH: setup channel --- this is for all addresses.
    nrf_put8_chk(n, NRF_RF_CH, nrf_default_channel);


    // reg=6: RF_SETUP: setup data rate and power.
    // datarate already has the right encoding.
    // 
    // use:
    //  - nrf_default_data_rate;
    //  - nrf_default_db;
    nrf_put8_chk(n, NRF_RF_SETUP, nrf_default_data_rate | nrf_default_db);

    // reg=7: status.  p59
    // sanity check that it is empty and nothing else is set.
    //
    // NOTE: if we are in the midst of an active system,
    // it's possible this node receives a message which will
    // change these values.   we might want to set the
    // rx addresses to something that won't get a message.
    // 
    // ideally we would do something where we use different 
    // addresses across reboots?   we get a bit of this benefit
    // by waiting the 100ms.
    uint8_t status = nrf_get8(n, NRF_STATUS);
    delay_ms(100);
    if (status != 0b00001110) {
        nrf_put8_chk(n, NRF_STATUS, 0b00001110);
    }


    // we skip reg=0x8 (observation)
    // we skip reg=0x9 (RPD)
    // we skip reg=0xA (P0)
    // we skip reg=0x10 (TX_ADDR): used only when sending.
    nrf_rx_flush(n);
    nrf_tx_flush(n);

    // i think w/ the nic is off, this better be true.
    assert(nrf_tx_fifo_empty(n));
    assert(nrf_rx_fifo_empty(n));


    // make sure you delay long enough
    // todo("go from <PowerDown> to <Standby-I>");
    // PWR_UP 1
    nrf_rx_mode(n);
    // ce_lo(c.ce_pin);
    // nrf_put8_chk(n, NRF_CONFIG, rx_config);
    // // todo("go from <Standby-I> to RX");
    // // CE 1
    // // gpio_write(c.ce_pin, 1);
    // ce_hi(c.ce_pin);
    // delay_ms(100);

// #endif


    // should be true after setup.
    // rx_config: 0x7f
    // nrf_get8(n, NRF_CONFIG): 0x2
    // printk("rx_config: %x\n", rx_config);
    // printk("nrf_get8(n, NRF_CONFIG): %x\n", nrf_get8(n, NRF_CONFIG));
    if(acked_p) {
        nrf_opt_assert(n, nrf_get8(n, NRF_CONFIG) == rx_config);
        nrf_opt_assert(n, nrf_pipe_is_enabled(n, 0));
        nrf_opt_assert(n, nrf_pipe_is_enabled(n, 1));
        nrf_opt_assert(n, nrf_pipe_is_acked(n, 0));
        nrf_opt_assert(n, nrf_pipe_is_acked(n, 1));
        nrf_opt_assert(n, nrf_tx_fifo_empty(n));
    } else {
        nrf_opt_assert(n, nrf_get8(n, NRF_CONFIG) == rx_config);
        nrf_opt_assert(n, !nrf_pipe_is_enabled(n, 0));
        nrf_opt_assert(n, nrf_pipe_is_enabled(n, 1));
        nrf_opt_assert(n, !nrf_pipe_is_acked(n, 1));
        nrf_opt_assert(n, nrf_tx_fifo_empty(n));
    }
    return n;
}

int nrf_tx_send_ack(nrf_t *n, uint32_t txaddr, 
    const void *msg, unsigned nbytes) {

    // default config for acked state.
    nrf_opt_assert(n, nrf_get8(n, NRF_CONFIG) == rx_config);
    nrf_opt_assert(n, nrf_pipe_is_enabled(n, 0));
    nrf_opt_assert(n, nrf_pipe_is_enabled(n, 1));
    nrf_opt_assert(n, nrf_pipe_is_acked(n, 0));
    nrf_opt_assert(n, nrf_pipe_is_acked(n, 1));
    nrf_opt_assert(n, nrf_tx_fifo_empty(n));

    // if interrupts not enabled: make sure we check for packets.
    while(nrf_get_pkts(n))
        ;

    // TODO: you would implement the rest of the code at this point.
    // see page 75 for transmit.  do the no_ack case first.
    // NOTE: 
    //  - have to set both NRF_RX_ADDR_P0 and NRF_TX_ADDR.
    //    alot more stuff.
    //  - put the data on FIFO before state transitioning.
    //  - if you get more than max_rt_interrupts
    //    you can panic for today.  but dump out the 
    //    configuration: good chance its a bad configure.
    int res = staff_nrf_tx_send_ack(n, txaddr, msg, nbytes);

    // int res = 0;
    // nrf_tx_mode(n);

    // // 2. set TX addr and pipe 0 for ack
    // nrf_set_addr(n, NRF_TX_ADDR, txaddr, nrf_default_addr_nbytes);

    // nrf_put8_chk(n, NRF_RX_PW_P0, nbytes);
    // nrf_set_addr(n, NRF_RX_ADDR_P0, txaddr, nrf_default_addr_nbytes);

    // // 3. write message to device
    // nrf_putn(n, NRF_W_TX_PAYLOAD, msg, nbytes);

    // // 4. pulse the CE pin to actually switch to TX mode
    // gpio_set_off(n->config.ce_pin);
    // delay_us(140);
    // gpio_set_on(n->config.ce_pin);
    // delay_us(140);

    // // 5. wait until the TX fifo is empty
    // while(!nrf_tx_fifo_empty(n))
    //     ;
    
    // // 6. check/clear the TX interrupt
    // if (nrf_has_tx_intr(n)) {
    //     res = nbytes;
    //     nrf_tx_intr_clr(n);
    // } else if (nrf_has_max_rt_intr(n)) {
    //     res = 0;
    //     nrf_rt_intr_clr(n);
    // }

    // // 7. set device back to RX mode
    // gpio_set_off(n->config.ce_pin);
    // delay_us(140);
    // nrf_put8_chk(n, NRF_CONFIG, rx_config);
    // gpio_set_on(n->config.ce_pin);
    // delay_us(140);

    // How to increment the total number of retransmissions.
    //  uint8_t cnt = nrf_get8(n, NRF_OBSERVE_TX);
    //  n->tot_retrans  += bits_get(cnt,0,3);

    // when done: tx interrupt better be cleared.
    nrf_opt_assert(n, !nrf_has_tx_intr(n));
    // when done: better be back in rx mode.
    nrf_opt_assert(n, nrf_get8(n, NRF_CONFIG) == rx_config);
    return res;
}

// send packet without hardware ack.
int nrf_tx_send_noack(nrf_t *n, uint32_t txaddr, 
    const void *msg, unsigned nbytes) {

    // default state for no-ack config.
    nrf_opt_assert(n, nrf_get8(n, NRF_CONFIG) == rx_config);
    nrf_opt_assert(n, !nrf_pipe_is_enabled(n, 0));
    nrf_opt_assert(n, nrf_pipe_is_enabled(n, 1));
    nrf_opt_assert(n, !nrf_pipe_is_acked(n, 1));
    nrf_opt_assert(n, nrf_tx_fifo_empty(n));

    // if interrupts not enabled: make sure we check for packets.
    while(nrf_get_pkts(n))
        ;

    // TODO: you would implement the send packet code.
    // see page 75.
    // 1. put packet on TX fifo.
    // 2. put NRF in TX mode, 
    // 3. wait for TX interrupt.
    // 4. assert that tx fifo is empty.
    // 5. Clear the tx interrupt.
    // 6. put back in rx mode.  (via standbyII->standbyI)
    // NOTE: 
    //   - If nRF24L01+ is in standby-II mode, it goes to 
    //     standby-I mode immediately if CE is set low.

    // // 1. set device to TX mode
    // nrf_put8_chk(n, NRF_CONFIG, tx_config);

    // // 2. set TX addr
    // nrf_set_addr(n, NRF_TX_ADDR, txaddr, nrf_default_addr_nbytes);

    // // 3. write message to device
    // nrf_putn(n, NRF_W_TX_PAYLOAD, msg, nbytes);

    // // 4. pulse the CE pin to actually switch to TX mode
    // gpio_set_off(n->config.ce_pin);
    // delay_us(140);
    // gpio_set_on(n->config.ce_pin);
    // delay_us(140);

    // // 5. wait until the TX fifo is empty
    // while(!nrf_tx_fifo_empty(n))
    //     ;
    
    // // 6. clear the TX interrupt
    // nrf_tx_intr_clr(n);

    // // 7. set device back to RX mode
    // gpio_set_off(n->config.ce_pin);
    // delay_us(140);
    // nrf_put8_chk(n, NRF_CONFIG, rx_config);
    // gpio_set_on(n->config.ce_pin);
    // delay_us(140);

    int res = staff_nrf_tx_send_noack(n, txaddr, msg, nbytes);

    // after done: tx interrupt better be cleared.
    nrf_opt_assert(n, !nrf_has_tx_intr(n));
    // after done: better be back in rx mode.
    nrf_opt_assert(n, nrf_get8(n, NRF_CONFIG) == rx_config);
    return res;
}

// while the RX fifo is not empty: read packets into the 
// circular queue <recvq> using <cq_push_n>. 
//
// return number of packets read.
//
// NOTE: 
//   - better be in RX mode!
//   - since we don't have interrupts on, we call this before doing
//     tx to make sure there is space and to reduce chance of dropping
//     packets.
//   - if you send packets you'll have to make sure the packets won't
//     get dropped b/c the receiver is not listening.  can use
//     interrupts.
int nrf_get_pkts(nrf_t *n) {
    // you can't check for packets unless in RX mode.
    nrf_opt_assert(n, nrf_get8(n, NRF_CONFIG) == rx_config);
    if(!nrf_rx_has_packet(n)) 
        return 0; 

    // TODO:
    // data sheet gives the sequence to follow to get packets.
    // p76: 
    // do {
    //    0. assert(pipeid from STATUS != PIPEID_EMPTY)
    //    1. read packet through spi [nrf_getn]
    //    2. push onto the receive queue [cq_push_n],
    //       increment <tot_recv_msgs> and <tot_recv_bytes>
    //    3. clear rx IRQ.
    //    4. read fifo status to see if more packets: 
    //       if so, repeat from (1) --- we need to do this now in case
    //       a packet arrives b/n (1) and (2)
    // } while (rx fifo is not empty)
    int res = staff_nrf_get_pkts(n);
    // while(!nrf_rx_fifo_empty(n)) {

    //     unsigned pipen = nrf_rx_get_pipeid(n);
    //     if(pipen == NRF_PIPEID_EMPTY)
    //         panic("impossible: empty pipeid: %b\n", pipen);
        
    //     uint8_t msg[NRF_PKT_MAX];

    //     uint8_t status = nrf_getn(n, NRF_R_RX_PAYLOAD, msg, n->config.nbytes);
    //     assert(pipeid_get(status) == pipen);

    //     if(!cq_push_n(&n->recvq, msg, n->config.nbytes))
    //         panic("not enough space left for message on pipe=%d\n", pipen);

    //     nrf_rx_intr_clr(n);
    //     res++;
    // }

    nrf_opt_assert(n, nrf_get8(n, NRF_CONFIG) == rx_config);
    return res;
}
