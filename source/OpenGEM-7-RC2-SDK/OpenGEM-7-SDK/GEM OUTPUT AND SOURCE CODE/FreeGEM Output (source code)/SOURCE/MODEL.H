;SMALL	equ	1
COMPACT equ	1
;MEDIUM  equ	1
;BIG  	equ	1
;LARGE	equ	1

ifdef	SMALL
ARG		equ	4
SMALL_CODE	equ	1
SMALL_DATA	equ	1
endif

ifdef	COMPACT
ARG		equ	4
SMALL_CODE	equ	1
LARGE_DATA	equ	1
endif

ifdef	MEDIUM
ARG		equ	6
LARGE_CODE	equ	1
SMALL_DATA	equ	1
endif

ifdef	BIG
ARG		equ	6
LARGE_CODE	equ	1
LARGE_DATA	equ	1
endif

ifdef	LARGE
ARG		equ	6
LARGE_CODE	equ	1
LARGE_DATA	equ	1
endif

