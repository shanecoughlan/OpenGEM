@echo off
@rem
@rem Generate GEM.CPI from four fonts
@rem
txt2psf gsans06.txt gsans06.psf
txt2psf gsans08.txt gsans08.psf
txt2psf gsans14.txt gsans14.psf
txt2psf gsans16.txt gsans16.psf

psfs2cpi --drfont --device=GEM @gemcpi.inp gem.cpi
psfs2cpi --drfont @gsans.inp gsans.cpi
rem psfs2cpi @gsans1.inp gsans1.cpi
rem psfs2cpi @gsans2.inp gsans2.cpi
rem psfs2cpi @gsans3.inp gsans3.cpi
copy gsans.cpi tmp.com
upx --8086 --best tmp.com
del gsans.cpx
ren tmp.com gsans.cpx
