%{

// $Id: sln.ll 1043 2008-07-25 12:00:41Z landrgr1 $
//
//  Copyright (c) 2008, Novartis Institutes for BioMedical Research Inc.
//  All rights reserved.
// 
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are
// met: 
//
//     * Redistributions of source code must retain the above copyright 
//       notice, this list of conditions and the following disclaimer.
//     * Redistributions in binary form must reproduce the above
//       copyright notice, this list of conditions and the following 
//       disclaimer in the documentation and/or other materials provided 
//       with the distribution.
//     * Neither the name of Novartis Institutes for BioMedical Research Inc. 
//       nor the names of its contributors may be used to endorse or promote 
//       products derived from this software without specific prior
//       written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
// "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
// LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
// A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
// OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
// SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
// LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
// DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
// THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
// OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//
// Created by Greg Landrum, September 2006
//

#include <cstdio>
#ifdef WIN32
#include <io.h> 	 
#endif

#include <GraphMol/RDKitBase.h>
#include <GraphMol/RDKitQueries.h>
#include <GraphMol/SLNParse/SLNAttribs.h>

#undef YY_INPUT

#include "InputFiller.h"

extern INPUT_FUNC_TYPE gp_myInput;

#define YY_INPUT( b, r, ms) (r = gp_myInput( b, ms ))

#include <string>
#include <cstring>
#include "sln.tab.hpp"

using namespace RDKit;
extern bool slnParserDoQueries;

%}

%option stack
%s IN_SLN_PARAM_STATE
%s IN_CTAB_PARAM_STATE
%s IN_PROP_VAL_STATE
%s IN_RECURSE_STATE
%%

[\r\n\t ]       return 0;

<IN_PROP_VAL_STATE>[^\;\]\>\&\|\!]* {
  yysln_lval.text_T=new std::string(yysln_text);
  return TEXT_BLOCK; 
}

<IN_CTAB_PARAM_STATE,IN_SLN_PARAM_STATE>[a-zA-Z]+[a-zA-Z0-9_\-,]* { 
  yysln_lval.text_T=new std::string(yysln_text);
  return TEXT_BLOCK; 
}

<INITIAL,IN_RECURSE_STATE>He |
<INITIAL,IN_RECURSE_STATE>Li |
<INITIAL,IN_RECURSE_STATE>Be |
<INITIAL,IN_RECURSE_STATE>Ne |
<INITIAL,IN_RECURSE_STATE>Na |
<INITIAL,IN_RECURSE_STATE>Mg |
<INITIAL,IN_RECURSE_STATE>Al |
<INITIAL,IN_RECURSE_STATE>Si |
<INITIAL,IN_RECURSE_STATE>Ar |
<INITIAL,IN_RECURSE_STATE>K |
<INITIAL,IN_RECURSE_STATE>Ca |
<INITIAL,IN_RECURSE_STATE>Sc |
<INITIAL,IN_RECURSE_STATE>Ti |
<INITIAL,IN_RECURSE_STATE>V |
<INITIAL,IN_RECURSE_STATE>Cr |
<INITIAL,IN_RECURSE_STATE>Mn |
<INITIAL,IN_RECURSE_STATE>Co |
<INITIAL,IN_RECURSE_STATE>Fe |
<INITIAL,IN_RECURSE_STATE>Ni |
<INITIAL,IN_RECURSE_STATE>Cu |
<INITIAL,IN_RECURSE_STATE>Zn |
<INITIAL,IN_RECURSE_STATE>Ga |
<INITIAL,IN_RECURSE_STATE>Ge |
<INITIAL,IN_RECURSE_STATE>As |
<INITIAL,IN_RECURSE_STATE>Se |
<INITIAL,IN_RECURSE_STATE>Kr |
<INITIAL,IN_RECURSE_STATE>Rb |
<INITIAL,IN_RECURSE_STATE>Sr |
<INITIAL,IN_RECURSE_STATE>Y |
<INITIAL,IN_RECURSE_STATE>Zr |
<INITIAL,IN_RECURSE_STATE>Nb |
<INITIAL,IN_RECURSE_STATE>Mo |
<INITIAL,IN_RECURSE_STATE>Tc |
<INITIAL,IN_RECURSE_STATE>Ru |
<INITIAL,IN_RECURSE_STATE>Rh |
<INITIAL,IN_RECURSE_STATE>Pd |
<INITIAL,IN_RECURSE_STATE>Ag |
<INITIAL,IN_RECURSE_STATE>Cd |
<INITIAL,IN_RECURSE_STATE>In |
<INITIAL,IN_RECURSE_STATE>Sn |
<INITIAL,IN_RECURSE_STATE>Sb |
<INITIAL,IN_RECURSE_STATE>Te |
<INITIAL,IN_RECURSE_STATE>Xe |
<INITIAL,IN_RECURSE_STATE>Cs |
<INITIAL,IN_RECURSE_STATE>Ba |
<INITIAL,IN_RECURSE_STATE>La |
<INITIAL,IN_RECURSE_STATE>Ce |
<INITIAL,IN_RECURSE_STATE>Pr |
<INITIAL,IN_RECURSE_STATE>Nd |
<INITIAL,IN_RECURSE_STATE>Pm |
<INITIAL,IN_RECURSE_STATE>Sm |
<INITIAL,IN_RECURSE_STATE>Eu |
<INITIAL,IN_RECURSE_STATE>Gd |
<INITIAL,IN_RECURSE_STATE>Tb |
<INITIAL,IN_RECURSE_STATE>Dy |
<INITIAL,IN_RECURSE_STATE>Ho |
<INITIAL,IN_RECURSE_STATE>Er |
<INITIAL,IN_RECURSE_STATE>Tm |
<INITIAL,IN_RECURSE_STATE>Yb |
<INITIAL,IN_RECURSE_STATE>Lu |
<INITIAL,IN_RECURSE_STATE>Hf |
<INITIAL,IN_RECURSE_STATE>Ta |
<INITIAL,IN_RECURSE_STATE>W |
<INITIAL,IN_RECURSE_STATE>Re |
<INITIAL,IN_RECURSE_STATE>Os |
<INITIAL,IN_RECURSE_STATE>Ir |
<INITIAL,IN_RECURSE_STATE>Pt |
<INITIAL,IN_RECURSE_STATE>Au |
<INITIAL,IN_RECURSE_STATE>Hg |
<INITIAL,IN_RECURSE_STATE>Tl |
<INITIAL,IN_RECURSE_STATE>Pb |
<INITIAL,IN_RECURSE_STATE>Bi |
<INITIAL,IN_RECURSE_STATE>Po |
<INITIAL,IN_RECURSE_STATE>At |
<INITIAL,IN_RECURSE_STATE>Rn |
<INITIAL,IN_RECURSE_STATE>Fr |
<INITIAL,IN_RECURSE_STATE>Ra |
<INITIAL,IN_RECURSE_STATE>Ac |
<INITIAL,IN_RECURSE_STATE>Th |
<INITIAL,IN_RECURSE_STATE>Pa |
<INITIAL,IN_RECURSE_STATE>U |
<INITIAL,IN_RECURSE_STATE>Np |
<INITIAL,IN_RECURSE_STATE>Pu |
<INITIAL,IN_RECURSE_STATE>Am |
<INITIAL,IN_RECURSE_STATE>Cm |
<INITIAL,IN_RECURSE_STATE>Bk |
<INITIAL,IN_RECURSE_STATE>Cf |
<INITIAL,IN_RECURSE_STATE>Es |
<INITIAL,IN_RECURSE_STATE>Fm |
<INITIAL,IN_RECURSE_STATE>Md |
<INITIAL,IN_RECURSE_STATE>No |
<INITIAL,IN_RECURSE_STATE>Lr |
<INITIAL,IN_RECURSE_STATE>B  |
<INITIAL,IN_RECURSE_STATE>C  |
<INITIAL,IN_RECURSE_STATE>N  |
<INITIAL,IN_RECURSE_STATE>O  |
<INITIAL,IN_RECURSE_STATE>P  |
<INITIAL,IN_RECURSE_STATE>S  |
<INITIAL,IN_RECURSE_STATE>F  |
<INITIAL,IN_RECURSE_STATE>Cl |
<INITIAL,IN_RECURSE_STATE>Br | 
<INITIAL,IN_RECURSE_STATE>I  {
  if(slnParserDoQueries){
          yysln_lval.atom_T = new QueryAtom(PeriodicTable::getTable()->getAtomicNumber(yytext));
        } else {
          yysln_lval.atom_T = new Atom(PeriodicTable::getTable()->getAtomicNumber(yytext));
        }
  // SLN has no concept of implicit Hs... they're either in the SLN or they don't exist:        
  yysln_lval.atom_T->setNoImplicit(true);
        
  return ATOM_TOKEN;
}
<INITIAL,IN_RECURSE_STATE>Any {
  if(slnParserDoQueries) {
    yysln_lval.atom_T = new QueryAtom();
    yysln_lval.atom_T->setQuery(makeAtomNullQuery());
  } else {
    yysln_lval.atom_T = new Atom(0);
  }
  // SLN has no concept of implicit Hs... they're either in the SLN or they don't exist:        
  yysln_lval.atom_T->setNoImplicit(true);
  return ATOM_TOKEN;
}

<INITIAL,IN_RECURSE_STATE>H { return H_TOKEN; }

<IN_SLN_PARAM_STATE>is\= |
<IN_SLN_PARAM_STATE>Is\= |
<IN_SLN_PARAM_STATE>iS\= |
<IN_SLN_PARAM_STATE>IS\= { yy_push_state(IN_RECURSE_STATE); return RECURSE_TOKEN; }
<IN_SLN_PARAM_STATE>not\= | 
<IN_SLN_PARAM_STATE>Not\= |
<IN_SLN_PARAM_STATE>nOt\= |
<IN_SLN_PARAM_STATE>noT\= |
<IN_SLN_PARAM_STATE>NOt\= |
<IN_SLN_PARAM_STATE>NoT\= |
<IN_SLN_PARAM_STATE>nOT\= |
<IN_SLN_PARAM_STATE>NOT\= { yy_push_state(IN_RECURSE_STATE); return NEG_RECURSE_TOKEN; }


\-                      { return MINUS_TOKEN; }

\+                      { return PLUS_TOKEN; }
\#                      { return HASH_TOKEN; }  
\~                      { return TILDE_TOKEN; } 
\:                      { return COLON_TOKEN; } 

\(              { return OPEN_PAREN_TOKEN; }
\)              { return CLOSE_PAREN_TOKEN; }

<IN_SLN_PARAM_STATE,IN_CTAB_PARAM_STATE>\!\= |
<IN_SLN_PARAM_STATE,IN_CTAB_PARAM_STATE>\>\= |
<IN_SLN_PARAM_STATE,IN_CTAB_PARAM_STATE>\<\= |
<IN_SLN_PARAM_STATE,IN_CTAB_PARAM_STATE>\= |
<IN_SLN_PARAM_STATE,IN_CTAB_PARAM_STATE>\> |
<IN_SLN_PARAM_STATE,IN_CTAB_PARAM_STATE>\< {
  yy_push_state(IN_PROP_VAL_STATE);
  yysln_lval.text_T=new std::string(yytext);
  return COMPARE_TOKEN; 
}

<INITIAL,IN_RECURSE_STATE>\= {
 return EQUALS_TOKEN; 
}

<IN_RECURSE_STATE>\; { 
  yy_pop_state(); 
  return SEMI_TOKEN; 
}
<IN_PROP_VAL_STATE>\; { yy_pop_state(); return SEMI_TOKEN; }
\; { return SEMI_TOKEN; }
<IN_PROP_VAL_STATE>\& { yy_pop_state(); return AND_TOKEN; }
<IN_PROP_VAL_STATE>\| { yy_pop_state(); return OR_TOKEN; }
\! { return NOT_TOKEN; }

\[                      { yy_push_state(IN_SLN_PARAM_STATE); return OPEN_BRACKET_TOKEN; }

<IN_RECURSE_STATE>\]           {
	// we're closing a recursive definition, which means we should also be
	//  closing a parameter block:
  yy_pop_state();
	if(YY_START!=IN_SLN_PARAM_STATE){
		std::cerr << " after closing a recursion, we were not in the appropriate state."  <<std::endl;
	} else {
    yy_pop_state();
  }
	return CLOSE_BRACKET_TOKEN;
}
<IN_PROP_VAL_STATE>\]           {
  // if we're currently in an SLN property block (e.g. in []'s), we need
  // to pop both the prop_val state and the property block state:
  yy_pop_state();
  if(YY_START==IN_SLN_PARAM_STATE) {
    yy_pop_state();
  } 
  return CLOSE_BRACKET_TOKEN;
}

<IN_SLN_PARAM_STATE>\]          { yy_pop_state(); return CLOSE_BRACKET_TOKEN; }

\.              { return SEPARATOR_TOKEN; }
<IN_RECURSE_STATE>\, { 
  return COMMA_TOKEN; 
}

\@              { return AT_TOKEN; }
\*              { return ASTERIX_TOKEN; }

[0-9]+  { yysln_lval.ival_T = atoi( yytext ); return NUMBER_TOKEN; }


<<EOF>>         { return EOS_TOKEN; }
.               return yytext[0];

%%
#if 0
// NOTE: this code belongs up above, but flex-- doesn't seem to
// provide the ability to comment stuff out in blocks.
\<                      { yy_push_state(IN_CTAB_PARAM_STATE); return OPEN_ANGLE_TOKEN; }
<IN_CTAB_PARAM_STATE>\> { yy_pop_state(); return CLOSE_ANGLE_TOKEN; }
<IN_PROP_VAL_STATE>\>   { 
  // if we're currently in a CTAB property block (e.g. in <>'s), we need
  // to pop both the prop_val state and the property block state:
  yy_pop_state();
  if(YY_START==IN_CTAB_PARAM_STATE) {
    yy_pop_state();
  } 
  return CLOSE_ANGLE_TOKEN; 
}
#endif

#undef yysln_wrap
int yysln_wrap( void ) { return 1; }




