{$M $800,0,19000}
{$IFDEF VER60 or VER70}
 {$G-} { XT - Compatibility }
{$ENDIF}
{$DEFINE BIG}
Program Sound_Driver_GEM;
uses dos;
{$IFNDEF BIF}
CONST BufSize:Word=16384; { Buffer size is only used in the WAV play routine }
{$ELSE}
CONST BufSize:Word=32768; { Buffer size is only used in the WAV play routine }
{$ENDIF}
      Version=$0004; { Version of Sounddriver }
      M_Stereo=1; { Stereo Possible }
      M_bit16=2;  { 16 Bit Output  }
      M_HighMem=4; { 2 Memory Buffers}
{
  Written by Heinz Rath,Peter Sieg

  Thanks to Peter Sieg for the correction of the bug which was the reason
  why the driver didn't find the soundcard.

  The following commands where originally written by Tapio Žij„l„
   Function InitSoundSystem(Base : Word) : Byte;
   Procedure WriteDSP(Data : Byte);
   Function ReadDSP : Byte;
   Procedure SpeakerOn;
   Procedure SpeakerOff;
   Procedure DMAStop;
   Procedure DMAContinue;
   Procedure Play(buffer:Pointer;Size:Word;Freq:Word;DMACh:Byte);
  The InitSoundSystem function i have changed a little bit.
  The original routines you find in file orig.pas.

  This is Free software under the License of GPL.
}
Const DmaChannel:Array [0..3,1..3] of Byte =(
      ($87,$0,$1),
      ($83,$2,$3),
      ($81,$2,$3),
      ($82,$6,$7));

var   basead,dma:Word;
      zyxg:string[90];
      blast,s:string[128];
      f:file;
      meme:Procedure;
      memory:POinter;
      b:char;
      ol,rate,dummy:word;
      oldvec:Procedure;
      re:Registers;
      intplay:Boolean;
      Reseta,ReadData,WriteData,DataAvailable:Word;

{$F+}
Function ReadDSP : Byte;
Begin
 While Port[basead+$0E] And $80 = 0 Do;
 ReadDSP := Port[basead+$0A];
End;
{$F-}
Function InitSoundSystem(Base : Word) : Byte;
var t,foo:integer;
Begin
 InitSoundSystem := 1;
 Reseta := Base + $06;
 ReadData := Base + $0A;
 WriteData := Base + $0C;
 DataAvailable := Base + $0E;
 Port[Reseta] := 1;
 { Originally here was "delay" used }
 for t:=1 to 8 do foo:=port[basead+$6];
 Port[Reseta] := 0;
 { Originally here was "delay" used }
 for t:=1 to 8 do foo:=port[basead+$6];
 If ReadDSP=$AA then InitSoundSystem := 0;
End;
{$F+}
Procedure ResetDSP;
var t,foo:Integer;
begin
 Port[basead+$6] := 1;
 for t:=1 to 8 do foo:=port[basead+$6];
 Port[basead+$6] := 0;
end;

Procedure WriteDSP(Data : Byte);
Begin
 While Port[WriteData] And $80 <> 0 Do;
 Port[WriteData] := Data;
End;

Procedure SpeakerOn;
Begin
 WriteDSP($D1);
End;

Procedure SpeakerOff;
Begin
 WriteDSP($D3);
End;

Procedure DMAStop;
Begin
 WriteDSP($D0);
End;

Procedure DMAContinue;
Begin
 WriteDSP($D4);
End;

Procedure Player(buffer:Pointer;Size:Word;Freq:Word;DMACh:Byte);
var ss,so:Word;
    Offset,Page: Word;
begin
 ss:=Seg(Buffer^);
 so:=Ofs(Buffer^);
 Dec(Size);
 Offset := ss Shl 4+so;
 Page := (ss+so Shr 4) Shr 12;
 Port[$0A]:= $4+DMACh;
 Port[$0C]:= 0;
 Port[$0B]:= $48+DMACh;
 Port[DMAChannel[DmaCh,2]]:=Lo(Offset);
 Port[DMAChannel[DmaCh,2]]:=Hi(Offset);
 Port[DMAChannel[DmaCh,1]]:=Page;
 Port[DMAChannel[DmaCh,3]]:=Lo(Size);
 Port[DMAChannel[DmaCh,3]]:=Hi(Size);
 Port[$0A]:=DMACh;
 WriteDSP($40);
 WriteDSP(256 - 1000000 Div Freq);
 WriteDSP($14);
 WriteDSP(Lo(Size));
 WriteDSP(Hi(Size));
End;
Procedure Wait(DmaCh:Byte);
var old,rest:Word;
    t,foo,ch:integer;
    l,h:Byte;
    fp:array[0..1] of byte;
    dmc:array[0..7] of byte;
begin
fp[0]:=$0C;
fp[1]:=$D8;
dmc[0]:=$01;
dmc[1]:=$03;
dmc[2]:=$05;
dmc[3]:=$07;
dmc[4]:=$C2;
dmc[5]:=$C6;
dmc[6]:=$CA;
dmc[7]:=$CE;
rest:=65535;
 ch:=DmaCh and $0007;
old:=rest;
repeat
 old:=rest;
 port[fp[ch div 4]]:=0;
 l:=port[dmc[ch]];
 h:=port[dmc[ch]];
 rest:=h*256+l;
 if rest=65535 then rest:=0;
 if rest>old then DMaCOntinue;
until (rest=0);
end;
Function WDma(DmaCh:Byte):Word;
var old,rest:Word;
    tr,t,foo,ch:integer;
    l,h:Byte;
    fp:array[0..1] of byte;
    dmc:array[0..7] of byte;
begin
fp[0]:=$0C;
fp[1]:=$D8;
dmc[0]:=$01;
dmc[1]:=$03;
dmc[2]:=$05;
dmc[3]:=$07;
dmc[4]:=$C2;
dmc[5]:=$C6;
dmc[6]:=$CA;
dmc[7]:=$CE;
rest:=65535;
 ch:=DmaCh and $0007;
old:=rest;
for tr:=1 to 3 do
 begin
 old:=rest;
 port[fp[ch div 4]]:=0;
 l:=port[dmc[ch]];
 h:=port[dmc[ch]];
 rest:=h*256+l;
 if rest=65535 then rest:=0;
 if rest>old then DMaCOntinue;
end;
wdma:=rest;
end;
Function MemStr(se,os:Word):String;
var i:Word;
    ch:Char;
    s:string;
begin
s:='';
repeat
 ch:=chr(mem[se:os]);
 if ch<>#0 then s:=s+ch; { For compatibility with C! }
 inc(os);
until ch=#0;
Memstr:=s;
end;
{$IFDEF BIF}
Function PlayIntr(fr:string):Boolean;
var dummy,khz:Word;
    rat,siz,l:LOngInt;
begin
 pos:=0;
 sd:=fr;
 assign(ir,fr);
 {$I-}
 reset(ir,1);
 {$I+}
 if ioresult<>0 then 
  begin
   playIntr:=False;
  end
 else
 begin
 Getmem(memory,bufsize);
  writeln(Seg(Memory^));
  writeln(ofs(Memory^));
  seek(ir,24);
  blockread(ir,rat,4,dummy);
  khz:=rat;
  seek(ir,44);
  SpeakerOn;
  IntPlay:=True;
  bh:=bufsize div 2;
  m1:=Ptr(seg(memory^),0000);
  m2:=Ptr(seg(memory^),bh);
  rate:=khz;
     blockread(ir,M1^,bh,d1); { Load sample }
     Player(M1,d1,rate,dma); { Play}
     blockread(ir,M2^,bh,d2); { Load sample }
  me:=1;
  PlayIntr:=true;
 end;
end;
{Procedure IPlay;INterrupt;}
procedure Iplay(Flags,CS,IP,AX,BX,CX,DX,SI,DI,DS,ES,BP:Word);interrupt;
var r:Word;
begin
 if intplay=True then
  begin
{   writeln('*');}
   ol:=0;
   r:=wdma(dma);
   if r=ol then r:=0;
   if r=0 then
    begin
     port[$20]:=$20;
{     writeln('1');}
(*     DmaStop; { Stop DMA or we crash }*)
{     writeln('2.1');}
     intplay:=False;
{  writeln(Seg(Memory^));
  writeln(ofs(Memory^)); }
(*     blockread(ir,Memory^,bh,dummy); { Load sample }*)
     intplay:=True;
{     writeln('3');}
     port[$20]:=$20;
     if me=1 then
      begin
       Player(M2,d2,rate,dma); { Play}
       blockread(ir,M1^,bh,d1); { Load sample }
       me:=2;
      end
     else
      begin
       Player(M1,d1,rate,dma); { Play}
       blockread(ir,M2^,bh,d2); { Load sample }
       me:=1;
      end;
{     writeln('4');}
     port[$20]:=$20;
     if (d1<>bh) or (d2<>bh) then
      begin
       SpeakerOff;
       DmaStop;
       intplay:=False;
       pos:=0;
       Freemem(memory,BufSize);
      end;
    end;
   ol:=r;
  end;
(*inline ($9C); { PUSHF -- Push flags }
{ Call old ISR using saved vector }
OldVec;*)
call(Vec);
end;
{$ELSE}
Function PlayIntr(fr:string):Boolean;
var dummy,khz:Word;
    rat,siz,l:LOngInt;
begin
 assign(f,fr);
 {$I-}
 reset(f,1);
 {$I+}
 if ioresult<>0 then 
  begin
   playIntr:=False;
  end
 else
 begin
 Getmem(memory,bufsize);
  seek(f,24);
  blockread(f,rat,4,dummy);
  khz:=rat;
  seek(f,44);
  SpeakerOn;
  IntPlay:=True;
  rate:=khz;
     blockread(f,Memory^,bufsize,dummy); { Load sample }
     Player(Memory,dummy,rate,dma); { Play}
  PlayIntr:=true;
 end;
end;
Procedure IPlay;INterrupt;
var r:Word;
begin
 if intplay=True then
  begin
   ol:=0;
   r:=wdma(dma);
   if r=ol then r:=0;
   if r=0 then
    begin
     Port[$20]:=$20;
     blockread(f,Memory^,bufsize,dummy); { Load sample }
     Player(Memory,dummy,rate,dma); { Play}
(*     DmaStop; { Stop DMA or we crash } *)
     if dummy<>bufsize then
      begin
       SpeakerOff;
       DmaStop;
       intplay:=False;
       Freemem(memory,BufSize);
      end;
    end;
   ol:=r;
  end;
inline ($9C); { PUSHF -- Push flags }
{ Call old ISR using saved vector }
OldVec;
end;
{$ENDIF}
Function PlayFile(fr:string):Boolean;
var dummy,khz:Word;
    rat,siz,l:LOngInt;
begin
 inline($FB); { STI }
 assign(f,fr);
 {$I-}
 reset(f,1);
 {$I+}
 if ioresult<>0 then 
  begin
   inline($FA); { CLI }
   Playfile:=False;
  end
 else
 begin
 Getmem(memory,bufsize);
  seek(f,24);
  blockread(f,rat,4,dummy);
  khz:=rat;
  seek(f,44);
  SpeakerOn;
  siz:=filesize(f);
  l:=44;
  repeat
   blockread(f,Memory^,bufsize,dummy); { Load sample }
   inc(l,dummy);
   Player(Memory,dummy,khz,dma); { Play}
   Wait(dma); { Wait until dma is ready }
   DmaStop; { Stop DMA or we crash }
  until (l>=siz);
  close(f);
  SpeakerOff;
  DmaStop;
  Freemem(memory,BufSize);
 end;
inline($FA); { CLI }
PlayFile:=True;
end;
(*Procedure RecordS(buffer:Pointer;Size:Word;Freq:Word;DMACh,qu:Byte);
var ss,so:Word;
    Offset,Page: Word;
    c,q:Byte;
begin
 {
   qu....Wird ignoriert
 }
 ss:=Seg(Buffer^);
 so:=Ofs(Buffer^);
 Dec(Size);
 Offset := ss Shl 4+so;
 Page := (ss+so Shr 4) Shr 12;
 Port[$0A]:= $4+DMACh;
 Port[$0C]:= 0;
 Port[$0B]:= ($44)+DMACh;
 Port[DMAChannel[1,2]]:=Lo(Offset);
 Port[DMAChannel[1,2]]:=Hi(Offset);
 Port[DMAChannel[1,1]]:=Page;
 Port[DMAChannel[1,3]]:=Lo(Size);
 Port[DMAChannel[1,3]]:=Hi(Size);
 Port[$0A]:=DMACh;
 SpeakerOFF;
 WriteDSP($40);
 WriteDSP(256 - 1000000 Div Freq);
 WriteDSP($24);
 WriteDSP(lO(size));
 WriteDSP(hi(size));
End;*)
Procedure Stereo(oN:word);
begin
end;
Procedure Volume(vol:Word);
begin
end;
procedure SndProc(Flags,CS,IP,AX,BX,CX,DX,SI,DI,DS,ES,BP:Word);interrupt;
var l:Boolean;
begin
if ax=$2208 then { ID For SndCall (Magic Bytes) }
 begin
  case BX of
   1 :
    begin
     CX:=Version;{ Version of driver }
     DX:=$1;     { Type of driver }
     AX:=basead; { Soundblaster port }
     SI:=Dma;    { Dma Channel }
     DI:=0;      { No Stereo }
    end;
{   2 :} { Is not implemented now }
   3 : SpeakerOn; { Speaker On }
   4 : SpeakerOff; { Speaker Off }
   5 : { Plays a Wav file }
    begin
     if PlayFile(MemStr(Es,Cx))=True then cx:=0 { Okay }
      else cx:=1; { error }
    end;
   6 : Player(Ptr(eS,si),CX,DX,dma); { Plays a sample }
   7 : Wait(DMa); {WaitForDma}
   8 : DMAStop; { StopDMA}
   9 :
    begin
     if Dx<>1 then Stereo(CX)
      else Dx:=0;
    end;
  10 : Volume(Cx); { Volume }
(*  11 : RecordS(Ptr(eS,si),CX,DX,dma,di); { Records a sample }*)
  13 : 
    begin
     if PlayIntr(MemStr(Es,Cx))=True then cx:=0 { Okay }
      else cx:=1; { error }
    end;
  14 :
    begin
      if intplay=True then cx:=1
       else cx:=0;
    end;
  15 :
   begin
    if intplay=True then
     begin
      SpeakerOff;
      DmaStop;
      intplay:=False;
      Freemem(memory,BufSize);
     end;
   end;
 end;
 end;
{inline($9C);} {PUSHF }
{oldvec;}
end;
{$F-}
Function DSPVersion:Word;
var h,l:Byte;
begin
 WriteDSP($E1);
 h:=ReadDSP;
 l:=ReadDSP;
 Dspversion:=(h*256+l);
end;
Procedure DetectSB;
var addr:array [1..6] of word;
    i:integer;
    add:string[5];
    cmask:Byte;
    bd:Word;
begin
 addr[1]:=$210;
 addr[2]:=$220;
 addr[3]:=$230;
 addr[4]:=$240;
 addr[5]:=$250;
 addr[6]:=$260;
 bd:=0;
 for i:=1 to 6 do
  begin
   basead:=addr[i];
   ResetDsp;
   if readdsp=$AA then bd:=addr[i];
  end;
basead:=bd;
if basead>0 then blast:=''
 else blast:='*';
end;
Procedure Init;
var blast:string;
    i:integer;
begin
 blast:=GetEnv('BLASTER');
 if blast='' then DetectSB;
 if blast='*' then
  begin
   writelN('No Soundblaster found!');
   halt(9);
  end;
 if blast='' then
  begin
   writelN('BLASTER environment variable not found!');
   halt(9);
  end;
 s:='$000';
 for i:=1 to length(blast) do
  begin
   blast[i]:=upcase(blast[i]);
   if blast[i]='A' then { Base adress of Sound Blaster }
    begin
     s[2]:=blast[i+1];
     s[3]:=blast[i+2];
     s[4]:=blast[i+3];
     val(s,basead,dummy);
    end;
   if blast[i]='D' then dma:=ord(blast[i+1])-ord('0'); { Dma Channel }
  end;
 If InitSoundSystem(basead) <> 0 then
  begin
   writeln('Initializing of soundcard failed!');
   halt(99);
  end;
 IF DspVersion>=$300 then
  begin
   writeln('WARNING! This driver does not fully use your card!');
   if DspVersion<$400 then writeln('Please use the BDSBPRO driver');
   if DspVersion>=$400 then writeln('Please use the BDSB16 driver');
  end;
end;
Begin
zyxg:='zyxg'+#0+#0+'GEM Sound driver v0.4 for Sound Blaster                                          ';
intplay:=False;
Init;
GetIntVec($1C,@oldvec);
memw[$000:($E1*4)+2]:=seg(sndproc);
memw[$000:($E1*4)]:=ofs(sndproc);
memw[$000:($1C*4)+2]:=seg(Iplay);
memw[$000:($1C*4)]:=ofs(Iplay);
GetIntVec($E1,@meme);
memory:=addr(sndproc);
writeln(Copy(zyxg,7,length(zyxg)-7));
write('Installation...');
if (seg(memory^)=seg(meme)) and (ofs(memory^)=ofs(meme)) then writelN('Okay')
 else
  begin
   writeln('Failed');
   halt(1);
  end;
write('Initializing...');
re.ax:=$2208;
re.bx:=1;
intr($E1,re);
if re.cx=Version then
 begin
  writeln('Okay');
  keep(1);
 end
 else
  begin
   writeln('Failed');
   writeln('No driver installed');
   halt(1);
  end;
End.
