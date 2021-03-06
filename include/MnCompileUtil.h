#pragma once

#define cm_throw(msg)		(throw MnException(msg))
#define cm_scan_error(msg)		(cm_throw( OU::string::format("scan error -> [row:%d,col:%d] %s",row,col,OvString(msg).c_str()) ) )
#define cm_parse_error(msg)		(cm_throw( OU::string::format("parse error -> [row:%d,col:%d] %s",cm_tok.row,cm_tok.col,OvString(msg).c_str()) ) )

#define cm_tok				( ( cm->tokpos<cm->tokens.size() )? cm->tokens[cm->tokpos] : CmToken() )
#define cm_lah/*lookahead*/	( ((cm->tokpos+1)<(cm->tokens.size()))? cm->tokens[cm->tokpos+1] : CmToken() )
#define cm_savepos()		( cm->savepos.push_back( cm->tokpos ), (cm->savepos.size()-1) )
#define cm_loadpos(idx)		{ if (idx < cm->savepos.size()) {cm->tokpos = cm->savepos.at(idx);cm->savepos.resize(idx);} }
#define cm_removesave(idx)	{ if (idx < cm->savepos.size()) cm->savepos.resize(idx); }

#define cm_lahmatch(t)		( (cm_lah.type) == (t) )

#define cm_tokerror()		( cm_parse_error("unexpected token\n"),false)
#define cm_tokmatch(t)		( (cm_tok.type) == (t) )
#define cm_tokoption(t)		( cm_tokmatch(t)? (cm_toknext(),true) : false )
#define cm_tokmust(t)		( cm_tokmatch(t)? true : cm_tokerror() )

#define cm_kwmatch(kw)		( cm_tokmatch(tt_identifier) && (MnToString(cm_tok.val)->str() == (kw)) )
#define cm_kwoption(kw)		( cm_kwmatch((kw))? (cm_toknext(),true) : false )
#define cm_kwmust(kw)		( cm_kwmatch(kw)?  true : cm_tokerror() )

#define cm_toknext()		( ++cm->tokpos )
#define cm_tokprev()		( --cm->tokpos )

#define cm_compile(stat)   ( statement::stat::compile(cm) )

#define cm_expr				(cm->exprinfo)
#define cm_tostack()		(statement::tostack(cm))
#define cm_assign(lexpr)	(statement::assign(cm,lexpr))
#define cm_free_expr()		(statement::free_expr(cm))
#define cm_resolve_goto(fi)	(statement::resolve_goto(cm,fi))
#define cm_resolve_break(bi) (statement::resolve_break(cm,bi))
#define cm_resolve_continue(ci) (statement::resolve_continue(cm,ci))

#define cm_code				(cm->fi->codewriter)
#define cm_codesize()		(cm->fi->func->code.size())
#define cm_getcode(pos)		(&(cm->fi->func->code.at((pos))))

#define cm_jumping()		(statement::jumping(cm,op_jump))
#define cm_fjumping()		(statement::jumping(cm,op_fjump))
#define cm_fixjump(pos,tpos) ( *(OvInt*)cm_getcode((pos)) = ((tpos) - (pos+sizeof(OvInt))) )

#define cm_addconst(val)	(statement::addconst(cm,(val)))
#define cm_addlabel(l)	    (cm->fi->labels.push_back((l)))
#define cm_addgoto(g)	    (cm->fi->gotos.push_back((g)))


OvInt CmScaning( CmCompiler* cm, OvInputStream* is )
{
	OvChar c;
	is->Read(c);
	OvBool eos = false;
	OvUInt row = 1;
	OvUInt col = 1;

#define cm_read() { c = is->Read(c)? c:OvEOS; ++col; if(c=='\n') { col=0; ++row; };}
#define cm_check_escape_char() (c = (c=='a')? '\a':(c=='b')? '\b':(c=='r')? '\r':(c=='"')? '\"':(c=='f')? '\f':(c=='t')? '\t':(c=='n')? '\n':(c=='0')? '\0':(c=='v')? '\v':(c=='\'')? '\'':(c=='\\')? '\\': c)

	while ( true )
	{
		if ( c == OvEOS )
		{
			cm->tokens.push_back( CmToken( tt_eos, row, col ) );
			return 0;
		}
		else if ( isdigit(c) )
		{
			CmToken tok( tt_number, row, col );
			OvReal num = 0;
			OvInt mult = 10;
			if ( c == '0' )
			{
				mult = 8;
				cm_read();
				if ( c == 'x' || c == 'X' )
				{
					mult = 16;
					cm_read();
				}
			}
			bool under = false;
			while ( true )
			{
				if ( isdigit(c) ) num = (num * mult) + (c-'0'); else break;
				cm_read();
			}

			tok.val = MnValue( num );
			cm->tokens.push_back( tok );
		}
		else if ( c == '"' )
		{
			CmToken tok( tt_string, row, col );
			OvString str;
			cm_read();
			do
			{
				if ( c == '\n' ) cm_scan_error( "abnormal string termination\n  " );
				if ( c == '\\') { cm_read(); cm_check_escape_char(); }
				str.push_back(c);
				cm_read();
			} while ( (c != '"') && (c != EOF) );
			cm_read();

			tok.val = ut_newstring( cm->s, str);
			cm->tokens.push_back( tok );
		}
		else if ( c == '\'' )
		{
			cm_read();
			if ( c == '\\') { cm_read(); cm_check_escape_char(); }
			CmToken tok( tt_number, row, col );
			tok.val = MnValue( (MnNumber)c );
			cm->tokens.push_back( tok );
			cm_read();
			if ( c != '\'' ) cm_scan_error( "abnormal character termination\n" );
			cm_read();
		}
		else if ( isalpha(c) || c == '_' )
		{
			CmToken tok( tt_identifier, row, col );
			OvString str;
			do
			{
				str.push_back(c);
				cm_read();
			} while ( (isalnum(c) || c == '_') );

			tok.val = ut_newstring( cm->s, str);
			cm->tokens.push_back( tok );
		}
		else if ( isspace( c ) )
		{
			while ( isspace( c ) ) cm_read();
		}
		else
		{
			OvChar d = c;
			CmToken tok( (CmTokenType)d, row, col );
			cm_read();
			if ( d=='+' && c=='+' ) {tok.val = ut_newstring(cm->s,"++"); tok.type = tt_identifier; cm_read(); }
			else if ( d=='-' && c=='-' ) {tok.val = ut_newstring(cm->s,"--"); tok.type = tt_identifier; cm_read(); }
			else if ( d==':' && c==':' ) {tok.val = ut_newstring(cm->s,"::"); tok.type = tt_identifier; cm_read(); }
			cm->tokens.push_back( tok );
		}
	}
#undef cm_read
	return 0;
}

//////////////////////////////////////////////////////////////////////////

OvBool statement::option( CmCompiler* cm, statfunc func )
{
	OvUInt saveidx = cm_savepos();
	OvBool ret = func(cm);
	if ( !ret ) {cm_loadpos(saveidx);} else {cm_removesave(saveidx);}
	return ret;
}

OvBool	statement::match( CmCompiler* cm, statfunc func )
{
	OvUInt saveidx = cm_savepos();
	OvBool ret = func(cm);
	cm_loadpos(saveidx);
	return ret;
}

void	statement::tostack( CmCompiler* cm )
{
	switch ( cm_expr.type )
	{
	case et_nil : cm_code << op_const_nil ; break;
	case et_const : cm_code << op_const << cm_expr.byte1 ; break;
	case et_boolean : cm_code << (cm_expr.blr? op_const_true : op_const_false); break;
	case et_number : 
		{
			if ( cm_expr.num == 0.0 ) cm_code << op_const_zero ;
			else if ( cm_expr.num == 1.0 ) cm_code << op_const_one ;
			else cm_code << op_const_num << cm_expr.num ;
		}
		break;

	case et_global : cm_code << op_getglobal << cm_expr.short1; break;
	case et_local : cm_code << op_getstack << cm_expr.short1; break;
	case et_upval : cm_code << op_getupval << cm_expr.byte1; break;
	case et_field : cm_code << op_getfield; break;

	case et_call : cm_code << op_call << cm_expr.byte1 << (OvByte)1; break;
	case et_onstack : return ;
	}
	cm_expr.type = et_onstack;
}

void	statement::assign( CmCompiler* cm, const CmExprInfo& lexpr )
{
	switch ( lexpr.type )
	{
	case et_global : cm_code << op_setglobal << lexpr.short1; break;
	case et_local : cm_code << op_setstack << lexpr.short1 ; break;
	case et_upval : cm_code << op_setupval << lexpr.byte1 ; break;
	case et_field : cm_code << op_setfield ; break;
	default: cm_parse_error( "'=' : left operand must be l-value\n" );
	}
	cm_expr = lexpr;
}

void	statement::free_expr( CmCompiler* cm )
{
	switch ( cm_expr.type )
	{
	case et_closure : cm_code << op_popstack << (OvShort)1; break;
	case et_field : cm_code << op_popstack << (OvShort)2; break;
	case et_onstack : cm_code << op_popstack << (OvShort)1; break;
	case et_call : cm_code << op_call << cm_expr.byte1 << (OvByte)0; break;
	}
	cm_expr.type = et_none;
}

OvByte	statement::addconst( CmCompiler* cm, const MnValue& val )
{
	MnIndex idx = cm->fi->func->consts.size();
	while ( idx-- )
	{
		const MnValue& cst = cm->fi->func->consts[idx];
		if ( MnToObject(val) == MnToObject(cst) ) return idx+1;
	}
	if ( cm->fi->func->consts.size() > 255 ) cm_parse_error( "there is too many constancies\n" );
	cm->fi->func->consts.push_back(val);
	return cm->fi->func->consts.size();
}

OvInt	statement::jumping( CmCompiler* cm, OvByte op )
{
	if ( op!=op_jump && op!=op_fjump ) cm_parse_error("invalid jump operation");
	cm_code << op ;
	OvUInt jump = cm_codesize();
	cm_code << OvInt(0);
	return jump;
}

void	statement::var_search( CmCompiler* cm, CmFuncinfo* fi, const MnValue& name )
{
	if ( fi )
	{
		OvHash32 hash = MnToString(name)->hash();
		MnIndex idx = fi->locals.size();
		while ( idx-- ) { if (fi->locals[idx] == hash) break; }

		if ( idx >= 0 )
		{
			cm_expr.type = et_local;
			cm_expr.short1	 = idx + 1;
			return;
		}
		else
		{
			var_search( cm, fi->last, name );
			if ( cm_expr.type != et_global )
			{
				CmUpvalInfo info;
				info.isupval = (cm_expr.type == et_upval);
				info.idx	 = info.isupval? cm_expr.byte1:cm_expr.short1;

				fi->upvals.push_back( info );
				cm_expr.type  = et_upval;
				cm_expr.byte1 = fi->upvals.size();
				cm_expr.byte2 = 0;
			}
			return;
		}
	}
	cm_expr.type  = et_global;
	cm_expr.byte1 = cm_addconst(name);
	cm_expr.byte2 = 0;
}

void statement::resolve_goto( CmCompiler* cm, CmFuncinfo* fi ) 
{
	for each ( const CmGotoInfo& igoto in fi->gotos )
	{
		OvBool solved = false;
		for each ( const CmLabelInfo& ilabel in fi->labels )
		{
			if ( igoto.name->hash() == ilabel.name->hash() )
			{
				cm_fixjump( igoto.pos, ilabel.pos );
				solved = true;
				break;
			}
		}
		if ( !solved ) cm_parse_error( igoto.name->str() + " - undefined label\n" );
	}
}
void statement::resolve_break( CmCompiler* cm, CmBreakInfo* bi )
{
	if ( !bi ) return;
	for each ( OvInt brk in bi->breaks ) cm_fixjump(brk,bi->out);
	bi->breaks.clear();
}

void	statement::resolve_continue( CmCompiler* cm, CmContinueInfo* ci )
{
	if ( !ci ) return;
	for each ( OvInt ctn in ci->continues ) cm_fixjump(ctn,ci->ret);
	ci->continues.clear();
}

//////////////////////////////////////////////////////////////////////////

void	statement::multi_stat::compile( CmCompiler* cm )
{
	while ( !cm_tokmatch(tt_eos) && !cm_tokmatch('}') )
	{
		cm_compile(single_stat);
	}
}

//////////////////////////////////////////////////////////////////////////

// 	else if ( cm_statoption(block) ) return true;
// 	else if ( cm_statoption(returnstat) ) return true;
// 	else if ( cm_statoption(ifstat) ) return true;
// 	else if ( cm_statoption(whilestat) ) return true;
// 	else if ( (cm_statoption(expression) && cm_tokmust(';')) ) return true;

void	statement::single_stat::compile( CmCompiler* cm )
{
	if ( cm_kwmatch("local") ) { cm_compile(local); }
	else if ( cm_kwmatch("if") ) { cm_compile(if_stat); }
	else if ( cm_kwmatch("while") ) { cm_compile(whilestat); }
	else if ( cm_kwmatch("break") ) { cm_compile(breakstat); }
	else if ( cm_kwmatch("continue") ) { cm_compile(continuestat); }
	else if ( cm_kwmatch("label") ) { cm_compile(label_stat); }
	else if ( cm_kwmatch("goto") ) { cm_compile(goto_stat); }
	else if ( cm_kwmatch("return") ) { cm_compile(return_stat); }
	else if ( cm_tokmatch('{') ) { cm_compile(block); }
	else
	{
		cm_compile(expression);
		cm_free_expr();
		if (cm_tokmust(';')) cm_toknext();
	}

	if (cm_tokmatch(';')) cm_toknext();
}
//////////////////////////////////////////////////////////////////////////

void	statement::label_stat::compile( CmCompiler* cm )
{
	cm_toknext();
	cm_tokmust(tt_identifier);
	cm_addlabel( CmLabelInfo( MnToString(cm_tok.val), cm_codesize() ) );
	cm_toknext();
	if (cm_tokmust(':')) cm_toknext();
}

//////////////////////////////////////////////////////////////////////////

void	statement::goto_stat::compile( CmCompiler* cm )
{
	cm_toknext();
	cm_tokmust(tt_identifier);

	CmGotoInfo igoto;
	igoto.pos = cm_jumping();
	igoto.name = MnToString(cm_tok.val);
	cm_addgoto(igoto);
	cm_toknext();
	if (cm_tokmust(';')) cm_toknext();
}

//////////////////////////////////////////////////////////////////////////

void	statement::return_stat::compile( CmCompiler* cm )
{
	if ( cm_kwmust("return") ) cm_toknext();
	cm_compile(expression);
	cm_tostack();
	cm_code << op_return << (OvByte)1;
	if ( cm_tokmust(';') ) cm_toknext();
}

//////////////////////////////////////////////////////////////////////////

void	statement::local::compile( CmCompiler* cm )
{
	if ( cm_kwmatch("local") ) cm_toknext(); else return ;
	if ( !cm_tokmust(tt_identifier) ) return ;

	cm->fi->locals.push_back( MnToString(cm_tok.val)->hash() );
	cm_code << op_const_nil ;
	cm_compile(expression);
	if (cm_tokmust(';')) cm_toknext();
}

//////////////////////////////////////////////////////////////////////////

void	statement::expression::compile( CmCompiler* cm )
{
	cm_compile(expr10);
}

//////////////////////////////////////////////////////////////////////////

// while ( cm_tokoption('=') )
// {
// }

void	statement::expr10::compile( CmCompiler* cm )
{
	cm_compile(expr9);
	CmExprInfo lexpr = cm_expr;
	while ( cm_tokoption('=') && !cm_lahmatch('=') )
	{
		cm_compile(expr9);
		cm_tostack();
		cm_assign(lexpr);
	}
}
//////////////////////////////////////////////////////////////////////////

void	statement::expr9::compile( CmCompiler* cm )
{
	cm_compile(expr8);
	while ( 1 )
	{
		if ( cm_tokmatch('=') && cm_lahmatch('=') )
		{
			cm_toknext();cm_toknext();
			cm_tostack();
			cm_compile(expr8);
			cm_tostack();
			cm_code << op_eq << op_eq;
		}
		else if ( cm_tokmatch('!') && cm_lahmatch('=') )
		{
			cm_toknext();cm_toknext();
			cm_tostack();
			cm_compile(expr8);
			cm_tostack();
			cm_code << op_eq << op_not;
		}
		else if ( (cm_tokmatch('>')||cm_tokmatch('<')) && !(cm_lahmatch('>')||cm_lahmatch('<')) )
		{
			opcode op = cm_tokmatch('>')? op_gt : op_lt;
			if ( cm_lahmatch('=') )
			{
				cm_toknext();cm_toknext();
				cm_tostack();
				cm_compile(expr8);
				cm_tostack();
				cm_code << op_eq << op;
			}
			else
			{
				cm_toknext();
				cm_tostack();
				cm_compile(expr8);
				cm_tostack();
				cm_code << op;				
			}
		}
		else break;
	}
}
//////////////////////////////////////////////////////////////////////////

void	statement::expr8::compile( CmCompiler* cm )
{
	cm_compile(expr7);
	while ( cm_tokmatch('|') && cm_lahmatch('|') )
	{
		cm_tostack();
		cm_toknext();cm_toknext();
		cm_compile(expr7);
		cm_tostack();
		cm_code << op_or;
	}
}
//////////////////////////////////////////////////////////////////////////

void	statement::expr7::compile( CmCompiler* cm )
{
	cm_compile(expr6);
	while ( cm_tokmatch('&') && cm_lahmatch('&') )
	{
		cm_tostack();
		cm_toknext();cm_toknext();
		cm_compile(expr6);
		cm_tostack();
		cm_code << op_and;
	}
}
//////////////////////////////////////////////////////////////////////////

void	statement::expr6::compile( CmCompiler* cm )
{
	cm_compile(expr5);
	while ( cm_tokmatch('|') && !cm_lahmatch('|') )
	{
		cm_tostack();
		cm_toknext();
		cm_compile(expr5);
		cm_tostack();
		cm_code << op_bit_or;
	}
}
//////////////////////////////////////////////////////////////////////////

void	statement::expr5::compile( CmCompiler* cm )
{
	cm_compile(expr4);
	while ( cm_tokmatch('^') )
	{
		cm_tostack();
		cm_toknext();
		cm_compile(expr4);
		cm_tostack();
		cm_code << op_bit_xor;
	}
}
//////////////////////////////////////////////////////////////////////////

void	statement::expr4::compile( CmCompiler* cm )
{
	cm_compile(expr3);
	while ( cm_tokmatch('&') && !cm_lahmatch('&') )
	{
		cm_tostack();
		cm_toknext();
		cm_compile(expr3);
		cm_tostack();
		cm_code << op_bit_and;
	}
}
//////////////////////////////////////////////////////////////////////////

void	statement::expr3::compile( CmCompiler* cm )
{
	cm_compile(expr2);
	while ( (cm_tokmatch('>') && cm_lahmatch('>')) || (cm_tokmatch('<') && cm_lahmatch('<')) )
	{
		cm_tostack();
		opcode op = cm_tokmatch('>')? op_pull : op_push;
		cm_toknext();cm_toknext();
		cm_compile(expr2);
		cm_tostack();
		cm_code << op;
	}
}
//////////////////////////////////////////////////////////////////////////

void	statement::expr2::compile( CmCompiler* cm )
{
	cm_compile(expr1);
	while ( cm_tokmatch('+') || cm_tokmatch('-') )
	{
		cm_tostack();
		opcode op = cm_tokmatch('+')? op_add : op_sub;
		cm_toknext();
		cm_compile(expr1);
		cm_tostack();
		cm_code << op;
	}
}
//////////////////////////////////////////////////////////////////////////

void	statement::expr1::compile( CmCompiler* cm )
{
	cm_compile(term);
	while ( (cm_tokmatch('*') || cm_tokmatch('/') || cm_tokmatch('%')) )
	{
		cm_tostack();
		opcode op = cm_tokmatch('*')? op_mul : cm_tokmatch('/')? op_div : op_mod;
		cm_toknext();
		cm_compile(term);
		cm_tostack();
		cm_code << op;
	}
}

//////////////////////////////////////////////////////////////////////////

void	statement::term::compile( CmCompiler* cm )
{
	cm_compile(preexpr);
}

//////////////////////////////////////////////////////////////////////////

void	statement::preexpr::compile( CmCompiler* cm )
{
	opcode op = cm_kwmatch("++")? op_inc:cm_kwmatch("--")? op_dec:op_none;
	if ( op != op_none ) cm_toknext();
	if ( op ==op_none ) cm_compile(postexpr); else cm_compile(preexpr);
	if ( op != op_none )
	{
		switch ( cm_expr.type )
		{
		case et_local : cm_code << op_getstack << cm_expr.short1; break;
		case et_global : cm_code << op_getglobal << cm_expr.short1; break;
		case et_upval : cm_code << op_getupval << cm_expr.byte1; break;
		case et_field :
			{
				cm_code << op_getstack << (OvShort)-2;
				cm_code << op_getstack << (OvShort)-2;
				cm_code << op_getfield ;
			}
			break;
		default:
			{
				if (op==op_inc) cm_parse_error( "pre '++' operator needs l-value\n" );
				else cm_parse_error( "pre '--' operator needs l-value\n" );
			}
		}
		cm_code << op ;
		cm_assign(cm_expr);
	}
}

//////////////////////////////////////////////////////////////////////////

void	statement::postexpr::compile( CmCompiler* cm )
{
	cm_compile(primary);
	while (1)
	{
		if ( cm_tokmatch('(') )
		{
			cm_tostack();
			cm_toknext();

			OvByte nargs = 0;
			while ( !cm_tokmatch(')') )
			{
				++nargs;
				cm_compile(expression);
				cm_tostack();
				if ( cm_tokmatch(',') ) cm_toknext();
				else if ( cm_tokmatch(')') ) break;
				else cm_tokerror();
			}
			cm_toknext(); 
			cm_expr.type = et_call;
			cm_expr.byte1  = nargs;
		}
		else if ( cm_tokmatch(':') )
		{
			cm_toknext();
			cm_tostack();
			cm_code << op_getstack << (OvShort)-1 ;
			if ( !cm_tokmatch(tt_identifier) ) cm_parse_error("'.' right must be field name\n");
			cm_code << op_const << cm_addconst(cm_tok.val);
			cm_toknext();
			
			cm_code << op_getfield ;
			cm_code << op_insertstack << (OvShort)-2;
			if ( cm_tokmust('(') ) cm_toknext();
			OvByte nargs = 0;
			while ( !cm_tokmatch(')') )
			{
				++nargs;
				cm_compile(expression);
				cm_tostack();
				if ( cm_tokmatch(',') ) cm_toknext();
				else if ( cm_tokmatch(')') ) break;
				else cm_tokerror();
			}
			cm_toknext(); 

			cm_expr.type = et_call;
			cm_expr.byte1  = nargs + 1;
		}
		else if ( cm_tokmatch('[') )
		{
			cm_tostack();
			cm_toknext();
			cm_compile(expression);
			cm_tostack();
			if (cm_tokmust(']')) cm_toknext();
			cm_expr.type = et_field;
		}
		else if ( cm_tokmatch('.') )
		{
			cm_tostack();
			cm_toknext();
			if ( !cm_tokmatch(tt_identifier) ) cm_parse_error("'.' right must be field name\n");
			cm_code << op_const << cm_addconst(cm_tok.val);
			cm_toknext();
			cm_expr.type = et_field;
		}
		else
		{
			if ( cm_tokmatch('+') && cm_lahmatch('+') ) cm_parse_error("post '++' operator isn't supported\n");
			if ( cm_tokmatch('-') && cm_lahmatch('-') ) cm_parse_error("post '--' operator isn't supported\n");
			break;
		}
	}
}

//////////////////////////////////////////////////////////////////////////

void	statement::primary::compile( CmCompiler* cm )
{
	if ( cm_tokmatch(tt_number) )
	{
		cm_expr.type = et_number;
		cm_expr.num = MnToNumber(cm_tok.val);
		cm_toknext();
	}
	else if ( cm_tokmatch(tt_string) )
	{
		cm_expr.type = et_const;
		cm_expr.byte1	 = cm_addconst(cm_tok.val);
		cm_toknext();
	}
	else if ( cm_kwmatch("function") )
	{
		cm_compile(funcdesc);
	}
	else if ( cm_kwmatch("nil") || cm_kwmatch("true") || cm_kwmatch("false") )
	{
		cm_expr.type = cm_kwmatch("nil")? et_nil : et_boolean;
		cm_expr.blr  = cm_kwmatch("true");
		cm_toknext();
	}
	else if ( cm_tokmatch(tt_identifier) || (cm_kwmatch("::")) )
	{
		cm_compile(variable);
	}
	else if ( cm_tokmatch('(') )
	{
		cm_toknext();
		cm_compile(expression);
		if ( cm_tokmust(')') ) cm_toknext();
	}
}

void	statement::variable::compile( CmCompiler* cm )
{
	if (cm_kwmatch("::"))
	{
		cm_expr.byte2 = 0;
		while ( cm_kwmatch("::") )
		{
			++cm_expr.byte2;
			cm_toknext();
		}
		cm_expr.type  = et_global;
		cm_tokmust(tt_identifier);
		cm_expr.byte1 = cm_addconst(cm_tok.val);
		cm_toknext();
	}
	else if ( cm_tokmatch(tt_identifier) )
	{
		var_search( cm, cm->fi, cm_tok.val );
		cm_toknext();
	}
	else
	{
		cm_expr.type = et_none;
	}
}
//////////////////////////////////////////////////////////////////////////

// 	if ( cm_kwoption("function") )
// 	{
// 		cm_toknext();
// 		cm_tokoption(tt_identifier);
// 		return cm_statmust(funcargs) && cm_statmust(block);
// 	}

void	statement::funcdesc::compile( CmCompiler* cm )
{
	cm_toknext();

	OvBool needthis = false;
	cm_compile(variable);
	if ( cm_expr.type != et_none && (cm_tokmatch(':') || cm_tokmatch('.')) )
	{
		needthis = cm_tokmatch(':');
		cm_toknext();
		cm_tokmust(tt_identifier);
		cm_tostack();
		cm_code << op_const << cm_addconst(cm_tok.val);
		cm_toknext();
		cm_expr.type = et_field;
	}
	CmExprInfo name = cm_expr;

	MnValue func = ut_newfunction(cm->s);
	CmFuncinfo ifi;
	ifi.func = MnToFunction(func);
	ifi.codewriter.func = ifi.func;
	ifi.last = cm->fi;
	cm->fi = &ifi;

	if (needthis) cm->fi->locals.push_back( OU::string::rs_hash( "this" ) );
	cm_compile(funcargs);

	if (cm_tokmust('{')) cm_toknext();
	cm_compile(funcbody);
	if (cm_tokmust('}')) cm_toknext();

	cm->fi = ifi.last;

	cm_code << op_newclosure << cm_addconst( func ) << (OvByte)ifi.upvals.size();
	for each ( const CmUpvalInfo& info in ifi.upvals )
	{
		cm_code << (info.isupval? op_getupval:op_getstack);
		if ( info.isupval ) cm_code << (OvByte)info.idx;
		else cm_code << info.idx;
	}

	cm_expr.type = et_closure;
	if ( name.type != et_none ) cm_assign(name);
}

//////////////////////////////////////////////////////////////////////////

void	statement::funcargs::compile( CmCompiler* cm )
{
	if (cm_tokmust('(')) cm_toknext();
	while ( cm_tokmatch(tt_identifier) )
	{
		cm->fi->locals.push_back( MnToString(cm_tok.val)->hash() );
		cm_toknext();
		if ( cm_tokmatch(',') ) cm_toknext(); else break;
	}
	if (cm_tokmust(')')) cm_toknext();
}

//////////////////////////////////////////////////////////////////////////

void statement::funcbody::compile( CmCompiler* cm )
{
	cm_compile(multi_stat);
	cm_code << op_return << (OvByte)0 ;
	cm_resolve_goto(cm->fi);
}

//////////////////////////////////////////////////////////////////////////

// 	if (cm_tokoption('('))
// 	{
// 		while ( true )
// 		{
// 			if (cm_statoption(expression) && cm_tokoption(',')) continue; else break;
// 		}
// 		return cm_tokmust(')');
// 	}

void	statement::callargs::compile( CmCompiler* cm )
{

}
//////////////////////////////////////////////////////////////////////////


// 	if (cm_kwoption("return"))
// 	{
// 		cm_toknext();
// 		return cm_statoption(expression) && cm_tokmust(';');
// 	}

void	statement::returnstat::compile( CmCompiler* cm )
{

}
//////////////////////////////////////////////////////////////////////////

// 	if (cm_kwoption("if"))
// 	{
// 		cm_toknext();
// 		OvBool ret = true;
// 		ret = ret && cm_tokmust('(');
// 		ret = ret && cm_statmust(expression);
// 		ret = ret && cm_tokmust(')');
// 		ret = ret && cm_statmust(single_stat);
// 		return ret;
// 	}

void	statement::if_stat::compile( CmCompiler* cm )
{
	cm_toknext();
	if ( cm_tokmust('(') ) cm_toknext();
	cm_compile(expression); cm_tostack();
	if ( cm_tokmust(')') ) cm_toknext();

	OvInt  jump1 = cm_fjumping();
	cm_compile(bodystat);
	OvInt  tpos1 = cm_codesize();

	if ( cm_kwmatch("else") )
	{
		cm_toknext();
		OvInt  jump2 = cm_jumping();
		tpos1 = cm_codesize();
		cm_compile(bodystat);
		OvInt  tpos2 = cm_codesize();
		cm_fixjump(jump2,tpos2);
	}

	cm_fixjump(jump1,tpos1);

}
//////////////////////////////////////////////////////////////////////////

// 	if (cm_kwoption("while"))
// 	{
// 		cm_toknext();
// 		OvBool ret = true;
// 		ret = ret && cm_tokmust('(');
// 		ret = ret && cm_statmust(expression);
// 		ret = ret && cm_tokmust(')');
// 		ret = ret && cm_statmust(single_stat);
// 		return ret;
// 	}

void	statement::whilestat::compile( CmCompiler* cm )
{
	cm_toknext();

	CmContinueInfo ci;
	CmContinueInfo* lastci = cm->ci;
	cm->ci = &ci;
	ci.ret = cm_codesize();

	if (cm_tokmust('(')) cm_toknext();
	cm_compile(expression);
	cm_tostack();
	if (cm_tokmust(')')) cm_toknext();

	CmBreakInfo bi;
	CmBreakInfo* lastbi = cm->bi;
	cm->bi = &bi;

	OvInt cond = cm_fjumping();

	OvInt nlocals = cm->fi->locals.size();
	cm_compile(bodystat);
	OvInt repeat = cm_jumping();
	cm->bi->out = cm_codesize();
	cm_code << op_settop << nlocals;
	OvInt tpos2 = cm_codesize();

	cm_fixjump(repeat,ci.ret);
	cm_fixjump(cond,tpos2);
	cm_resolve_break(cm->bi);
	cm_resolve_continue(cm->ci);

	cm->bi = lastbi;
	cm->ci = lastci;
}

//////////////////////////////////////////////////////////////////////////

void	statement::breakstat::compile( CmCompiler* cm )
{
	if (!cm->bi) cm_parse_error("invalid break\n");
	cm_toknext();
	cm->bi->breaks.push_back( cm_jumping() );
	cm_tokmust(';');
	cm_toknext();
}

//////////////////////////////////////////////////////////////////////////

void	statement::continuestat::compile( CmCompiler* cm )
{
	if (!cm->ci) cm_parse_error("invalid continue\n");
	cm_kwmust("continue");
	cm_toknext();
	cm->ci->continues.push_back( cm_jumping() );
	cm_tokmust(';');
	cm_toknext();
}

//////////////////////////////////////////////////////////////////////////

void	statement::block::compile( CmCompiler* cm )
{
	if (cm_tokmust('{')) cm_toknext();

	OvInt nlocals = cm->fi->locals.size();
	cm_compile(multi_stat);
	if ( nlocals < cm->fi->locals.size() )
	{
		cm_code << op_close_upval << OvShort(nlocals) ;
		cm_code << op_settop << nlocals;
		cm->fi->locals.resize(nlocals);
	}
	if (cm_tokmust('}')) cm_toknext();
}

//////////////////////////////////////////////////////////////////////////

void	statement::bodystat::compile( CmCompiler* cm )
{
	OvInt nlocals = cm->fi->locals.size();
	cm_compile(single_stat);
	if ( nlocals < cm->fi->locals.size() )
	{
		cm_code << op_close_upval << OvShort(nlocals) ;
		cm_code << op_settop << nlocals;
		cm->fi->locals.resize(nlocals);
	}
}