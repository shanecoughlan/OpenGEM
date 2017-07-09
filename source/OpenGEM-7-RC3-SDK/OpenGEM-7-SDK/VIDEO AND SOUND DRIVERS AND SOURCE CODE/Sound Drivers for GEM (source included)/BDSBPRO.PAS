{$M $800,0,24000}
{$IFDEF VER60 or VER70}
{$G-} { XT - Compatibility }
{$ENDIF}
Program Sound_Driver_GEM;
uses dos;

CONST BufSize:Word=16384; { Buffer size is only used in the WAV play routine }
      M_Stereo=1; { Stereo Possible }
      M_bit16=2;  { 16 Bit Output  }
      Version=$0004; { Version of Sounddriver }
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

var   dsp_ver,main,basead,dma:Word;
      zyxg:string[90];
      oldVec:Procedure;
      blast,s:string[128];
      f:file;
      meme:procedure;
      memory:POinter;
      b:char;
      IntPlay,st:Boolean;
      re:registers;
      ol,rate,ver,dummy:word;
      Reseta,ReadData,WriteData,DataAvailable:Word;

Function ReadDSP : Byte;
Begin
 While Port[basead+$0E] And $80 = 0 Do;
 ReadDSP := Port[basead+$0A];
End;

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

Procedure Player(buffer:Pointer;Size:Word;Freq:Word;DMACh:Byte);
var ss,so:Word;
    Offset,Page: Word;
begin
 ResetDSP;
 ss:=Seg(Buffer^);
 so:=Ofs(Buffer^);
 Dec(Size);
 Offset := ss Shl 4+so;
 Page := (ss+so Shr 4) Shr 12;
 Port[$0A]:= $4+DMACh;
 Port[$0C]:= 0;
 Port[$0B]:= $48+DMACh;
 Port[DMAChannel[DmaCH,2]]:=Lo(Offset);
 Port[DMAChannel[DmaCH,2]]:=Hi(Offset);
 Port[DMAChannel[DmaCH,1]]:=Page;
 Port[DMAChannel[DmaCH,3]]:=Lo(Size);
 Port[DMAChannel[DmaCH,3]]:=Hi(Size);
 Port[$0A]:=DMACh;
 SpeakerON;
 WriteDSP($40);
 WriteDSP(256 - 1000000 Div Freq);
 WriteDSP($14);
 WriteDSP(lO(size));
 WriteDSP(hi(size));
End;
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
Function PlayFile(fr:string):Boolean;
var dummy,khz:Word;
    rat,siz,l:LOngInt;
begin
 assign(f,fr);
 {$I-}
 reset(f,1);
 {$I+}
 if ioresult<>0 then 
  begin
   Playfile:=False;
  end
 else
 begin
 Getmem(memory,bufsize);
  seek(f,24);
  blockread(f,rat,4,dummy);
  khz:=Rat;
  seek(f,44);
  SpeakerOn;
  siz:=filesize(f);
  l:=44;
  repeat
   blockread(f,Memory^,bufsize,dummy); { Load sample }
   inc(l,dummy);
(*   Wait(dma); { Only for secuirty }*)
   Player(Memory,dummy,khz,dma); { Play}
   Wait(dma); { Wait until dma is ready }
   DmaStop;
  until (l>=siz);
  close(f);
  ResetDSP;
  SpeakerOff;
  DmaStop;
  Freemem(memory,BufSize);
 end;
PlayFile:=True;
end;
Procedure Stereo(oN:word);
begin
 if On=1 then
  begin
   st:=true;
   port[basead+$4]:=$e;
   port[basead+$5]:=port[basead+$5] or 2;
  end
 else
  begin
   st:=False;
   port[basead+$4]:=$e;
   port[basead+$5]:=port[basead+$5] and $FD;
  end;
end;
Procedure UnInst;
begin
{$IFDEF VER50 or VER55}
(* inline($BB/main/    { mov ax,main }
        $8E/$C0/     { mov es,ax }
        $89/$C1/     { mov cx,ax }
        {??????}     { mov es,es:[$2C] }
        $B4/$49/     { mov ah,$49 }
        $CD/$21/     { int 21h }
        $8E/$C1/     { mov es,cx }
        $B4/$49/     { mov ah,49 }
        $CD/$21/     { int 21h }
       );*)
{$ENDIF}
{$IFDEF VER60 or VER70}
asm
 sti
 mov ax,main
 mov es,ax
 mov cx,ax
 mov es,es:[$2C]
 mov ah,$49
 int 21h
 mov es,cx
 mov ah,$49
 int $21
 cli
end;
{$ENDIF}
end;
Function ReadMIx(Reg:Byte):Byte;
var we:byte;
begin
port[basead+$4]:=reg;
Readmix:=port[basead+$5]
end;
Procedure WriteMIx(Reg,we:byte);
begin
Port[Basead+$4]:=Reg;
Port[basead+$5]:=we;
end;
Procedure Volume(vol:Word);
begin
 port[basead+4]:=4;
 port[basead+5]:=vol;
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
     DmaStop;
     blockread(f,Memory^,bufsize,dummy); { Load sample }
     Player(Memory,dummy,rate,dma); { Play}
(*     DmaStop; { Stop DMA or we crash }*)
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
(*Procedure RecordS(buffer:Pointer;Size:Word;Freq:Word;DMACh,qu:Byte);
var ss,so:Word;
    Offset,Page: Word;
    c,q:Byte;
begin
 {
   qu....Quelle
   0 ....Mic
   1 ....Line IN
   2 ....CD
 }
 ss:=Seg(Buffer^);
 so:=Ofs(Buffer^);
 Dec(Size);
 Offset := ss Shl 4+so;
 Page := (ss+so Shr 4) Shr 12;
 Port[$0A]:= $4+DMACh;
 Port[$0C]:= 0;
 Port[$0B]:= ($40 or $10 or $4)+DMACh;
 Port[DMAChannel[1,2]]:=Lo(Offset);
 Port[DMAChannel[1,2]]:=Hi(Offset);
 Port[DMAChannel[1,1]]:=Page;
 Port[DMAChannel[1,3]]:=Lo(Size);
 Port[DMAChannel[1,3]]:=Hi(Size);
 Port[$0A]:=DMACh;
 SpeakerOFF; { Der lautsprecher mus aus sein !!}
 WriteDSP($A0); { Mono Modus }
 WriteDSP($40);
 WriteDSP(256 - 1000000 Div Freq);
  q:=ReadMix($0C);
  c:=c and not $06;
  if (qu=0) then c:=c or $00; { Mic }
  if (qu=1) then c:=c or $06; { Line In}
  if (qu=2) then c:=c or $02; { CD }
  if (qu<0) or (qu>2) then c:=c or $00;
 WRiteMix($0c,c);
 WriteDSP($24);
 WriteDSP(lO(size));
 WriteDSP(hi(size));
End;*)
procedure SndProc(Flags,CS,IP,AX,BX,CX,DX,SI,DI,DS,ES,BP:Word);interrupt;
begin
if ax=$2208 then { ID For SndCall (Magic Bytes) }
 begin
  case BX of
   1 :
    begin
     CX:=Version;    { Version of driver }
     DX:=$1;        { Type of driver }
     AX:=basead;   { Soundblaster port }
     SI:=Dma;      { Dma Channel }
     DI:=M_Stereo; { Stereo }
    end;
   2 : UnInst; { UnInstall Driver }
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
      else
       begin
        if st=true then cx:=1
         else cx:=0;
       end;
    end;
  10 : Volume(Cx);
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
 ver:=dspversion;
 if ver<$300 then
  begin
   writeln('Sorry! No Soundblaster PRO compatible card!');
   writelN;
   write('The following card was found:');
   if ver<$100 then writeln('Unknown');
   if (ver>=$100) and (ver<=$200) then writeln('Soundblaster or Soundblaster 1.5 or compatible');
   if (ver>=$201) and (ver<$300) then writeln('Soundblaster 2.0 or compatible');
   halt(99);
  end;
 IF DspVersion>=$400 then
  begin
   writeln('WARNING! This driver may work nor correct with your card!');
   writeln('Please use BDSB16 or a better driver');
  end;
end;
Begin
st:=False;
main:=Prefixseg;
IntPlay:=False;
zyxg:='zyxg'+#0+#0+'GEM Sound driver v0.4 for Sound Blaster pro                                      ';
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
