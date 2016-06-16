{

SYBREX SYSTEMS LIMITED
Copyright (C) 2005 - 2007 Sybrex Systems Ltd.
All rights reserved.

NOTICE: Sybrex permits you to use, modify, and distribute this file
in accordance with the terms of the Sybrex license agreement
accompanying it. If you have received this file from a source other
than Sybrex, then your use, modification, or distribution of it
requires the prior written permission of Sybrex.

}


{$ifdef DEBUGOFF}
{$D-,L-}
{$else}
{$D+,L+}
{$endif}

{$IFDEF VER110}
  {$DEFINE CB}
{$ENDIF}

{$IFDEF VER125}
  {$DEFINE CB}
{$ENDIF}

{$IFDEF VER130}
  {$IFDEF BCB}
    {$DEFINE CB}
  {$ENDIF}
{$ENDIF}

{$IFDEF BCB}
  {$DEFINE CB}
{$ENDIF}


{$ifdef CONDITIONALEXPRESSIONS}

  {$DEFINE VARIANTS}

  {$IF CompilerVersion >= 15}
    {$DEFINE V7ABOVE}
    {$WARN UNSAFE_CAST OFF}
  {$ifend}

  {$IF CompilerVersion >= 18.0}
    {$DEFINE V2Y}
  {$ifend}

  {$IF CompilerVersion >= 20.0}
    {$DEFINE UNI}
    {$DEFINE RTLINC}
  {$ifend}

  {$IF CompilerVersion >= 22.0}
    {$DEFINE XE}
  {$ifend}

  {$IF CompilerVersion >= 23.0}
    {$DEFINE W3264}
    {$define USENAMESPACE}
  {$ifend}

  {$IF CompilerVersion >= 25.0}
    {$DEFINE USEANSISTRINGS}
  {$ifend}

{$endif}


{$A+}
{$J+}
{$R-}
{$Q-}
