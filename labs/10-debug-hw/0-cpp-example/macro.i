#1 "macro.c"
#1 "<built-in>" 1
#1 "<built-in>" 3
#414 "<built-in>" 3
#1 "<command line>" 1
#1 "<built-in>" 2
#1 "macro.c" 2
#39 "macro.c"
static inline uint32_t 
cp14_debug_id_get(void)
{


	uint32_t	ret = 0;

	asm volatile    ("mrc p14, 0, %0, c0, c0, 0":"=r" (ret));
	return ret;
}






static inline uint32_t cp14_debug_id_macro_get(void){
	uint32_t	ret = 0;
	asm volatile    ("mrc " "p14" ", " "0" ", " "%0, " "c0" ", " "c0" ", " "0":"=r" (ret));
	return ret;
}
