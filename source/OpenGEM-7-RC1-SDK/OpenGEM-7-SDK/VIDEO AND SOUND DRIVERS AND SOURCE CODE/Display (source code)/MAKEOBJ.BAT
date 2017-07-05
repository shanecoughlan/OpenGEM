rasm86 accstart.a86 $pzsz
lc1 gembind -s
lc2 gembind -scode -v
rasm86 gemasm.a86 $pzsz
lc1 vdibind -s
lc2 vdibind -scode -v
lc1 dosbind -s
lc2 dosbind -scode -v
rasm86 dosasm.a86 $pzsz
rasm86 vdiasm.a86 $pzsz
rasm86 tcrtlasm.a86 $pzsz
rasm86 longasm.a86 $pzsz
rasm86 prostart.a86 $pzsz
rasm86 proend.a86 $pzsz
rasm86 dosif.a86 $pzsz
