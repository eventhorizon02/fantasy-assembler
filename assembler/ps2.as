! sample source file with errors
.entry LENGTH
.extern W
MAIN: 		miv		@r3, LENGTH
LOOP:	 		jmp 	L1
	  		prn 	-5
	  		bnx 	 W
	  		sub 	@r5,,@r4
	  		bne 	L3
L1:	  		inc 	K, 98
MOSHE: gjgl llghjgjjkgh7 88 @r5
.entry LOOP
	  		jmp		W
END:  		stop
STR:  		.string "abcdef"
LENGTH: 	.data	6,-9,15
K:			.data	22
.extern L3
