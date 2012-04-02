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
#define cm_tokmust(t)		( cm_tokoption(t)? true : cm_tokerror() )

#define cm_kwmatch(kw)		( cm_tokmatch(tt_identifier) && (MnToString(cm_tok.val)->get_str() == (kw)) )
#define cm_kwoption(kw)		( cm_kwmatch((kw))? (cm_toknext(),true) : false )
#define cm_kwmust(kw)		( cm_kwoption(kw)?  true : cm_tokerror() )

#define cm_toknext()		( ++cm->tokpos )
#define cm_tokprev()		( --cm->tokpos )

#define cm_compile(stat)   ( statement::stat::compile(cm) )


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
			CmToken tok( tt_const, row, col );
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
			CmToken tok( tt_const, row, col );
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
	cm->fi = &ifi;
	cm->level.push_back( cm->symbols.size() );
	cm_compile(multi_stat);
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

//////////////////////////////////////////////////////////////////////////

void	statement::multi_stat::compile( CmCompiler* cm )
{
	while ( cm_compile(single_stat) );
}

//////////////////////////////////////////////////////////////////////////

OvBool	statement::single_stat::compile( CmCompiler* cm )
{
	if ( cm_kwmatch("local") ) { cm_compile(local); return true; }
// 	else if ( cm_statoption(block) ) return true;
// 	else if ( cm_statoption(returnstat) ) return true;
// 	else if ( cm_statoption(ifstat) ) return true;
// 	else if ( cm_statoption(whilestat) ) return true;
// 	else if ( (cm_statoption(expression) && cm_tokmust(';')) ) return true;
	else if ( cm_tokoption(';') ) return true;
	else if ( cm_tokmatch(tt_eos) || cm_tokmatch('}')) return false;

	return cm_tokerror();
}

//////////////////////////////////////////////////////////////////////////

void	statement::local::compile( CmCompiler* cm )
{
	cm_kwmust("local");
	do cm_compile(addsymb); while ( cm_tokoption(',') );
	cm_compile(expression);
	cm_tokmust(';');
}

//////////////////////////////////////////////////////////////////////////

void statement::addsymb::compile( CmCompiler* cm )
{
	if (!cm_tokmatch(tt_identifier)) return;
	CmSyminfo symb;
	symb.hash = MnToString(cm_tok.val)->get_hash();
	symb.level = cm->fi->level;
	symb.index = cm->symbols.size() - cm->level[cm->fi->level];
	cm->symbols.push_back( symb );
	cm_toknext();
}

//////////////////////////////////////////////////////////////////////////

OvBool	statement::block::test( CmCompiler* cm )
{
	if ( cm_tokoption('{') )
	{
		return cm_tokmust('}');
	}
	return false;
}

void	statement::block::compile( CmCompiler* cm )
{
	cm_tokmatch(tt_identifier);
}

//////////////////////////////////////////////////////////////////////////

OvBool	statement::expression::test( CmCompiler* cm )
{
	return true;
}

void	statement::expression::compile( CmCompiler* cm )
{
	cm_tokmatch(tt_identifier);
}

//////////////////////////////////////////////////////////////////////////

OvBool	statement::expr10::test( CmCompiler* cm )
{
	while ( cm_tokoption('=') )
	{
	}
	return true;
}

//////////////////////////////////////////////////////////////////////////

OvBool	statement::expr9::test( CmCompiler* cm )
{
// 	OvBool ret = cm_statoption(expr8);
// 	while ( ret )
// 	{
// 		if ( (cm_tokmatch('!') || cm_tokmatch('=')) && cm_lahmatch('=') ) {cm_toknext();cm_toknext();}
// 		else if ( cm_tokoption('>') || cm_tokoption('<') ) cm_tokoption('=');
// 		else break;
// 		ret = ret && cm_statmust(expr8);
// 	}
	// 	return ret;
	return true;
}

//////////////////////////////////////////////////////////////////////////

OvBool	statement::expr8::test( CmCompiler* cm )
{
// 	OvBool ret = cm_statoption(expr7);
// 	while ( ret && (cm_tokoption('|') && cm_tokoption('|')) )
// 	{
// 		ret = ret && cm_statmust(expr7);
// 	}
// 	return ret;
	return true;
}

//////////////////////////////////////////////////////////////////////////

OvBool	statement::expr7::test( CmCompiler* cm )
{
// 	OvBool ret = cm_statoption(expr6);
// 	while ( ret && (cm_tokoption('&') && cm_tokoption('&')) )
// 	{
// 		ret = ret && cm_statmust(expr6);
// 	}
	// 	return ret;
	return true;
}

//////////////////////////////////////////////////////////////////////////

OvBool	statement::expr6::test( CmCompiler* cm )
{
// 	OvBool ret = cm_statoption(expr5);
// 	while ( ret && cm_tokoption('|') )
// 	{
// 		if ( cm_tokmatch('|') ) { cm_tokprev(); break; }
// 		ret = ret && cm_statmust(expr5);
// 	}
	// 	return ret;
	return true;
}

//////////////////////////////////////////////////////////////////////////

OvBool	statement::expr5::test( CmCompiler* cm )
{
// 	OvBool ret = cm_statoption(expr4);
// 	while ( ret && (cm_tokoption('^')) )
// 	{
// 		ret = ret && cm_statmust(expr4);
// 	}
	// 	return ret;
	return true;
}

//////////////////////////////////////////////////////////////////////////

OvBool	statement::expr4::test( CmCompiler* cm )
{
// 	OvBool ret = cm_statoption(expr3);
// 	while ( ret && cm_tokoption('&') )
// 	{
// 		if ( cm_tokmatch('&') ) { cm_tokprev(); break; }
// 		ret = ret && cm_statmust(expr3);
// 	}
	// 	return ret;
	return true;
}

//////////////////////////////////////////////////////////////////////////

OvBool	statement::expr3::test( CmCompiler* cm )
{
// 	OvBool ret = cm_statoption(expr2);
// 	while ( ret && (cm_tokmatch('>') && cm_lahmatch('>')) || (cm_tokmatch('<') && cm_lahmatch('<')) )
// 	{
// 		cm_toknext();cm_toknext();
// 		ret = ret && cm_statmust(expr2);
// 	}
	// 	return ret;
	return true;
}

//////////////////////////////////////////////////////////////////////////

OvBool	statement::expr2::test( CmCompiler* cm )
{
// 	OvBool ret = cm_statoption(expr1);
// 	while ( ret && (cm_tokoption('+') || cm_tokoption('-')) )
// 	{
// 		ret = ret && cm_statmust(expr1);
// 	}
	// 	return ret;
	return true;
}

//////////////////////////////////////////////////////////////////////////

void	statement::expr1::compile( CmCompiler* cm )
{
	cm_compile(term);
	while ( (cm_tokoption('*') || cm_tokoption('/') || cm_tokoption('%')) )
	{
	}
}

//////////////////////////////////////////////////////////////////////////

OvBool	statement::term::test( CmCompiler* cm )
{
	return true;
}

void	statement::term::compile( CmCompiler* cm )
{
	return cm_compile(postexpr);
}

//////////////////////////////////////////////////////////////////////////

OvBool	statement::postexpr::test( CmCompiler* cm )
{
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
	return false;
}

void	statement::postexpr::compile( CmCompiler* cm )
{
	return cm_compile(primary);
}

//////////////////////////////////////////////////////////////////////////

OvBool	statement::primary::test( CmCompiler* cm )
{
// 	if ( cm_statoption(funcdesc) ) return true;
// 	else if ( cm_tokoption(tt_const) ) return true;
// 	else if ( cm_tokoption(tt_identifier) ) return true;
// 	else if ( cm_tokoption('(') )
// 	{
// 		return cm_statmust(expression) && cm_tokmust(')');
// 	}
	return false;
}

void	statement::primary::compile( CmCompiler* cm )
{
	if ( cm_tokoption(tt_const) )
	{
		CmExpression expr;
		expr.type = et_const;
		expr.idx  = cs_const( cm->fi->func->consts.size() );
		cm->fi->func->consts.push_back( cm_tok.val );
	}
	else if ( cm_tokoption('(') )
	{
		cm_compile(expression);
		cm_tokmust(')');
	}
}

//////////////////////////////////////////////////////////////////////////

OvBool	statement::funcdesc::test( CmCompiler* cm )
{
// 	if ( cm_kwoption("function") )
// 	{
// 		cm_toknext();
// 		cm_tokoption(tt_identifier);
// 		return cm_statmust(funcargs) && cm_statmust(block);
// 	}
	return false;
}

//////////////////////////////////////////////////////////////////////////

OvBool	statement::funcargs::test( CmCompiler* cm )
{
	if ( cm_tokoption('(') )
	{
		while ( true )
		{
			if (cm_tokoption(tt_identifier) && cm_tokoption(',')) continue; else break;
		}
		return cm_tokmust(')');
	}
	return false;
}

//////////////////////////////////////////////////////////////////////////

OvBool	statement::callargs::test( CmCompiler* cm )
{
// 	if (cm_tokoption('('))
// 	{
// 		while ( true )
// 		{
// 			if (cm_statoption(expression) && cm_tokoption(',')) continue; else break;
// 		}
// 		return cm_tokmust(')');
// 	}
	return false;
}

//////////////////////////////////////////////////////////////////////////

OvBool	statement::returnstat::test( CmCompiler* cm )
{
// 	if (cm_kwoption("return"))
// 	{
// 		cm_toknext();
// 		return cm_statoption(expression) && cm_tokmust(';');
// 	}
	return false;
}

//////////////////////////////////////////////////////////////////////////

OvBool	statement::ifstat::test( CmCompiler* cm )
{
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
	return false;
}

//////////////////////////////////////////////////////////////////////////

OvBool	statement::whilestat::test( CmCompiler* cm )
{
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
	return false;
}

//////////////////////////////////////////////////////////////////////////