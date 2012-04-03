#pragma once

#define cm_tok				( ( cm->tokpos<cm->tokens.size() )? cm->tokens[cm->tokpos] : CmToken() )
#define cm_lah/*lookahead*/	( ((cm->tokpos+1)<(cm->tokens.size()))? cm->tokens[cm->tokpos+1] : CmToken() )
#define cm_savepos()		( cm->savepos.push_back( cm->tokpos ), (cm->savepos.size()-1) )
#define cm_loadpos(idx)		{ if (idx < cm->savepos.size()) {cm->tokpos = cm->savepos.at(idx);cm->savepos.resize(idx);} }
#define cm_removesave(idx)	{ if (idx < cm->savepos.size()) cm->savepos.resize(idx); }

#define cm_lahmatch(t)		( (cm_lah.type) == (t) )

#define cm_tokerror()		(printf( "token: %d, row: %d, col: %d\n", cm_tok.type, cm_tok.row, cm_tok.col ),false)
#define cm_tokmatch(t)		( (cm_tok.type) == (t) )
#define cm_tokoption(t)		( cm_tokmatch(t)? (cm_toknext(),true) : false )
#define cm_tokmust(t)		( cm_tokmatch(t)? true : cm_tokerror() )

#define cm_kwmatch(kw)		( cm_tokmatch(tt_identifier) && (MnToString(cm_tok.val)->str() == (kw)) )
#define cm_kwoption(kw)		( cm_kwmatch((kw))? (cm_toknext(),true) : false )
#define cm_kwmust(kw)		( cm_kwoption(kw)?  true : cm_tokerror() )

#define cm_toknext()		( ++cm->tokpos )
#define cm_tokprev()		( --cm->tokpos )

#define cm_compile(stat)   ( statement::stat::compile(cm) )

#define cm_expr				(cm->exprinfo)
#define cm_rvalue()			(statement::rvalue(cm))

#define cm_code				(cm->fi->codestream)

OvInt CmScaning( CmCompiler* cm, const OvString& file )
{
	OvFileInputStream fis( file );
	OvChar c;
	fis.Read(c);
	OvUInt row = 1;
	OvUInt col = 1;

#define cp_read() {fis.Read(c); ++col; if(c=='\n') { col=0; ++row; };}

	while ( true )
	{
		if ( c == EOF  )
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
				cp_read();
				if ( c == 'x' || c == 'X' )
				{
					mult = 16;
					cp_read();
				}
			}
			bool under = false;
			while ( true )
			{
				if ( isdigit(c) ) num = (num * mult) + (c-'0'); else break;
				cp_read();
			}

			tok.val = MnValue( num );
			cm->tokens.push_back( tok );
		}
		else if ( c == '"' )
		{
			CmToken tok( tt_string, row, col );
			OvString str;
			cp_read();
			do
			{
				str.push_back(c);
				cp_read();
			} while ( (c != '"') && (c != EOF) );
			cp_read();

			tok.val = MnValue( MOT_STRING, ut_newstring( cm->s, str) );
			cm->tokens.push_back( tok );
		}
		else if ( c == '\'' )
		{
			CmToken tok( tt_char, row, col );
			tok.val = MnValue( (MnNumber)c );
			cm->tokens.push_back( tok );
			cp_read();
		}
		else if ( isalpha(c) || c == '_' )
		{
			CmToken tok( tt_identifier, row, col );
			OvString str;
			do
			{
				str.push_back(c);
				cp_read();
			} while ( (isalnum(c) || c == '_') );

			tok.val = MnValue( MOT_STRING, ut_newstring( cm->s, str) );
			cm->tokens.push_back( tok );
		}
		else if ( isspace( c ) )
		{
			while ( isspace( c ) ) cp_read();
		}
		else
		{
			OvChar ret = c;
			CmToken tok( (CmTokenType)ret, row, col );
			cp_read();
			cm->tokens.push_back( tok );
		}
	}
#undef cp_read
	return 0;
}

void CmParsing( CmCompiler* cm );
void CmStatements( CmCompiler* cm );

void CmParsing( CmCompiler* cm )
{
	CmFuncinfo ifi;
	ifi.func = ut_newfunction(cm->s);
	ifi.codestream.Reset( ifi.func->code );

	cm->fi = &ifi;
	cm_compile(multi_stat);
	cm->fi->codestream << op_return;

	ut_excute_func( cm->s, cm->fi->func );
}

void CmStatements( CmCompiler* cm )
{
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

void	statement::rvalue( CmCompiler* cm )
{
	switch ( cm_expr.type )
	{
	case et_const : cm_code << op_const << cm_expr.idx ; break;
	case et_number : cm_code << op_const_num << cm_expr.num ; break;
	case et_boolean : cm_code << (cm_expr.blr? op_const_true : op_const_false); break;

	case et_local : cm_code << op_getstack << cm_expr.idx; break;
	case et_field : cm_code << op_getfield; break;
	case et_upval : cm_code << op_getupval << cm_expr.idx; break;
	case et_rvalue : return ;
	}
	cm_expr.type = et_rvalue;
}

//////////////////////////////////////////////////////////////////////////

void	statement::multi_stat::compile( CmCompiler* cm )
{
	while ( true )
	{
		cm_compile(single_stat);
		if ( cm_tokmatch(tt_eos) || cm_tokmatch('}')) return;
	}
}

//////////////////////////////////////////////////////////////////////////

void	statement::single_stat::compile( CmCompiler* cm )
{
	if ( cm_kwmatch("local") ) { cm_compile(local); }
// 	else if ( cm_statoption(block) ) return true;
// 	else if ( cm_statoption(returnstat) ) return true;
// 	else if ( cm_statoption(ifstat) ) return true;
// 	else if ( cm_statoption(whilestat) ) return true;
// 	else if ( (cm_statoption(expression) && cm_tokmust(';')) ) return true;
	else cm_compile(expression);
	cm_tokoption(';');
}

//////////////////////////////////////////////////////////////////////////

void	statement::local::compile( CmCompiler* cm )
{
	if ( cm_kwmatch("local") ) cm_toknext(); else return ;
	if ( !cm_tokmust(tt_identifier) ) return ;

	cm->fi->locals.push_back( MnToString(cm_tok.val)->hash() );
	cm_code << op_const_nil ;
	cm_compile(expression);
	cm_tokmust(';');
	cm_toknext();
}

//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////

void	statement::block::compile( CmCompiler* cm )
{
	cm_tokmatch(tt_identifier);
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
	while ( cm_tokoption('=') )
	{
		cm_compile(expr9);
		cm_rvalue();

		switch ( lexpr.type )
		{
		case et_local : cm_code << op_setstack << lexpr.idx ; break;
		case et_field : cm_code << op_setfield ; break;
		case et_upval : cm_code << op_setupval << lexpr.idx ; break;
		default: printf( "'=' : left operand must be l-value" );
		}
	}
}
//////////////////////////////////////////////////////////////////////////

// 	OvBool ret = cm_statoption(expr8);
// 	while ( ret )
// 	{
// 		if ( (cm_tokmatch('!') || cm_tokmatch('=')) && cm_lahmatch('=') ) {cm_toknext();cm_toknext();}
// 		else if ( cm_tokoption('>') || cm_tokoption('<') ) cm_tokoption('=');
// 		else break;
// 		ret = ret && cm_statmust(expr8);
// 	}
// 	return ret;

void	statement::expr9::compile( CmCompiler* cm )
{
	cm_compile(expr8);
}
//////////////////////////////////////////////////////////////////////////

// 	OvBool ret = cm_statoption(expr7);
// 	while ( ret && (cm_tokoption('|') && cm_tokoption('|')) )
// 	{
// 		ret = ret && cm_statmust(expr7);
// 	}
// 	return ret;

void	statement::expr8::compile( CmCompiler* cm )
{
	cm_compile(expr7);
}
//////////////////////////////////////////////////////////////////////////

// 	OvBool ret = cm_statoption(expr6);
// 	while ( ret && (cm_tokoption('&') && cm_tokoption('&')) )
// 	{
// 		ret = ret && cm_statmust(expr6);
// 	}
// 	return ret;
void	statement::expr7::compile( CmCompiler* cm )
{
	cm_compile(expr6);
}
//////////////////////////////////////////////////////////////////////////

// 	OvBool ret = cm_statoption(expr5);
// 	while ( ret && cm_tokoption('|') )
// 	{
// 		if ( cm_tokmatch('|') ) { cm_tokprev(); break; }
// 		ret = ret && cm_statmust(expr5);
// 	}
// 	return ret;

void	statement::expr6::compile( CmCompiler* cm )
{
	cm_compile(expr5);
}
//////////////////////////////////////////////////////////////////////////

// 	OvBool ret = cm_statoption(expr4);
// 	while ( ret && (cm_tokoption('^')) )
// 	{
// 		ret = ret && cm_statmust(expr4);
// 	}
// 	return ret;

void	statement::expr5::compile( CmCompiler* cm )
{
	cm_compile(expr4);
}
//////////////////////////////////////////////////////////////////////////

// 	OvBool ret = cm_statoption(expr3);
// 	while ( ret && cm_tokoption('&') )
// 	{
// 		if ( cm_tokmatch('&') ) { cm_tokprev(); break; }
// 		ret = ret && cm_statmust(expr3);
// 	}
// 	return ret;

void	statement::expr4::compile( CmCompiler* cm )
{
	cm_compile(expr3);
}
//////////////////////////////////////////////////////////////////////////

// 	OvBool ret = cm_statoption(expr2);
// 	while ( ret && (cm_tokmatch('>') && cm_lahmatch('>')) || (cm_tokmatch('<') && cm_lahmatch('<')) )
// 	{
// 		cm_toknext();cm_toknext();
// 		ret = ret && cm_statmust(expr2);
// 	}
// 	return ret;

void	statement::expr3::compile( CmCompiler* cm )
{
	cm_compile(expr2);
}
//////////////////////////////////////////////////////////////////////////

void	statement::expr2::compile( CmCompiler* cm )
{
	cm_compile(expr1);
	while ( cm_tokmatch('+') || cm_tokmatch('-') )
	{
		cm_rvalue();
		opcode op = cm_tokmatch('+')? op_add : op_sub;
		cm_toknext();
		cm_compile(expr1);
		cm_rvalue();
		cm_code << op;
	}
}
//////////////////////////////////////////////////////////////////////////

void	statement::expr1::compile( CmCompiler* cm )
{
	cm_compile(term);
	while ( (cm_tokmatch('*') || cm_tokmatch('/') || cm_tokmatch('%')) )
	{
		cm_rvalue();
		opcode op = cm_tokmatch('*')? op_mul : cm_tokmatch('/')? op_div : op_mod;
		cm_toknext();
		cm_compile(term);
		cm_rvalue();
		cm_code << op;
	}
}

//////////////////////////////////////////////////////////////////////////

void	statement::term::compile( CmCompiler* cm )
{
	cm_compile(postexpr);
}

//////////////////////////////////////////////////////////////////////////

// 	if ( cm_statoption(primary) )
// 	{
// 		while ( true )
// 		{
// 			if (cm_statoption(callargs)) continue;
// 			else if ( cm_tokmatch('.') && cm_lahmatch(tt_identifier) ) {cm_toknext();cm_toknext(); continue; }
// 			else if ( cm_tokoption('[') )
// 			{
// 				cm_statmust(expression);
// 				cm_tokmust(']');
// 				continue;
// 			}
// 			else break;
// 		}
// 		return true;
// 	}

void	statement::postexpr::compile( CmCompiler* cm )
{
	cm_compile(primary);
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
	else if ( cm_tokmatch(tt_identifier) )
	{
		OvHash32 hash = MnToString(cm_tok.val)->hash();
		MnIndex idx = cm->fi->locals.size();
		while ( idx-- ) { if (cm->fi->locals[idx] == hash) break; }

		cm_expr.type = et_local;
		cm_expr.idx	 = idx + 1;
		cm_toknext();
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

}
//////////////////////////////////////////////////////////////////////////

// if ( cm_tokoption('(') )
// {
// 	while ( true )
// 	{
// 		if (cm_tokoption(tt_identifier) && cm_tokoption(',')) continue; else break;
// 	}
// 	return cm_tokmust(')');
// }

void	statement::funcargs::compile( CmCompiler* cm )
{

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

void	statement::ifstat::compile( CmCompiler* cm )
{

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

}
//////////////////////////////////////////////////////////////////////////
