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
	cm->level.push_back( cm->locals.size() );
	cm_compile(multi_stat);
	cm->fi->codestream << op_return;
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
	symb.index = cm->locals.size() - cm->level[cm->fi->level];
	cm->locals.push_back( symb );
	cm_toknext();
}

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

// 	OvBool ret = cm_statoption(expr1);
// 	while ( ret && (cm_tokoption('+') || cm_tokoption('-')) )
// 	{
// 		ret = ret && cm_statmust(expr1);
// 	}
// 	return ret;

void	statement::expr2::compile( CmCompiler* cm )
{
	cm_compile(expr1);
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

void	statement::term::compile( CmCompiler* cm )
{
	return cm_compile(postexpr);
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

CmExprType	statement::primary::compile( CmCompiler* cm )
{
	if ( cm_tokoption(tt_number) )
	{
		cm->fi->codestream << (OvByte)op_const_num << MnToNumber(cm_tok.val);
		return et_const;
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