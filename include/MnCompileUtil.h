#pragma once

#define cm_tok				( ( cm->tokpos<cm->tokens.size() )? cm->tokens[cm->tokpos] : CmToken() )
#define cm_lookahead		( ((cm->tokpos+1)<(cm->tokens.size()))? cm->tokens[cm->tokpos+1] : CmToken() )
#define cm_savepos()		( cm->savepos.push_back( cm->tokpos ), (cm->savepos.size()-1) )
#define cm_loadpos(idx)		{ if (idx < cm->savepos.size()) {cm->tokpos = cm->savepos.back();cm->savepos.resize(idx);} }
#define cm_removesabe(idx)	{ if (idx < cm->savepos.size()) cm->savepos.resize(idx); }

#define cm_lahmatch(t)		( (cm_lookahead.type) == (t) )
#define cm_tokmatch(t)		( (cm_tok.type) == (t) )
#define cm_tokoption(t)		( cm_tokmatch(t)? (cm_toknext(),true) : false )
#define cm_tokmust(t)		( cm_tokoption(t)? true : (printf( "token: %d, row: %d, col: %d\n", t, cm_tok.row, cm_tok.col ),false) )
#define cm_tokkeyword(kw)	( cm_tokmatch(tt_identifier) && (MnToString(cm_tok.val)->get_str() == kw) )
#define cm_toknext()		( ++cm->tokpos )
#define cm_tokprev()		( --cm->tokpos )

#define cm_statmatch(stat)	(statement::match(cm,&statement::stat::test))
#define cm_statoption(stat)	(statement::option(cm,&statement::stat::test))
#define cm_statmust(stat)	(cm_statoption(stat)? true : (printf( "statement: %s, row: %d, col: %d\n", #stat, cm_tok.row, cm_tok.col ),false) )
#define cm_statparse(stat)	(statement::stat::parse(cm))

#define cm_errmessage(msg)	(printf(msg), false)

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
			} while ( c != '"' );
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
	CmStatements( cm );
}

void CmStatements( CmCompiler* cm )
{
	while ( cm_statoption(single_stat) );
}

//////////////////////////////////////////////////////////////////////////

OvBool statement::option( CmCompiler* cm, testfunc func )
{
	OvUInt saveidx = cm_savepos();
	OvBool ret = func(cm);
	if ( !ret ) {cm_loadpos(saveidx);} else {cm_removesabe(saveidx);}
	return ret;
}
OvBool	statement::match( CmCompiler* cm, testfunc func )
{
	OvUInt saveidx = cm_savepos();
	OvBool ret = func(cm);
	cm_loadpos(saveidx);
	return ret;
}

//////////////////////////////////////////////////////////////////////////

OvBool	statement::single_stat::test( CmCompiler* cm )
{
	return cm_statoption(local) ||
		(cm_statoption(expression) && cm_tokmust(';'));
}

OvBool	statement::single_stat::parse( CmCompiler* cm )
{
	return true;
}

//////////////////////////////////////////////////////////////////////////

OvBool	statement::local::test( CmCompiler* cm )
{
	OvBool ret = cm_tokkeyword("local");cm_toknext();
	ret = ret && cm_tokmust(tt_identifier);
	if ( cm_tokoption('=') )
	{
		ret = ret && cm_statmust(expression);
	}
	ret = ret && cm_tokmust(';');
	return ret;
}

OvBool	statement::local::parse( CmCompiler* cm )
{
	return true;
}

//////////////////////////////////////////////////////////////////////////

OvBool	statement::expression::test( CmCompiler* cm )
{
	return cm_statoption(expr10);
}

OvBool	statement::expression::parse( CmCompiler* cm )
{
	return cm_tokmatch(tt_identifier);
}

//////////////////////////////////////////////////////////////////////////

OvBool	statement::expr10::test( CmCompiler* cm )
{
	OvBool ret = cm_statoption(expr9);
	while ( ret && cm_tokoption('=') )
	{
		ret = ret && cm_statmust(expr9);
	}
	return ret;
}

//////////////////////////////////////////////////////////////////////////

OvBool	statement::expr9::test( CmCompiler* cm )
{
	OvBool ret = cm_statoption(expr8);
	while ( ret )
	{
		if ( (cm_tokmatch('!') || cm_tokmatch('=')) && cm_lahmatch('=') ) {cm_toknext();cm_toknext();}
		else if ( cm_tokoption('>') || cm_tokoption('<') ) cm_tokoption('=');
		else break;
		ret = ret && cm_statmust(expr8);
	}
	return ret;
}

//////////////////////////////////////////////////////////////////////////

OvBool	statement::expr8::test( CmCompiler* cm )
{
	OvBool ret = cm_statoption(expr7);
	while ( ret && (cm_tokoption('|') && cm_tokoption('|')) )
	{
		ret = ret && cm_statmust(expr7);
	}
	return ret;
}

//////////////////////////////////////////////////////////////////////////

OvBool	statement::expr7::test( CmCompiler* cm )
{
	OvBool ret = cm_statoption(expr6);
	while ( ret && (cm_tokoption('&') && cm_tokoption('&')) )
	{
		ret = ret && cm_statmust(expr6);
	}
	return ret;
}

//////////////////////////////////////////////////////////////////////////

OvBool	statement::expr6::test( CmCompiler* cm )
{
	OvBool ret = cm_statoption(expr5);
	while ( ret && cm_tokoption('|') )
	{
		if ( cm_tokmatch('|') ) { cm_tokprev(); break; }
		ret = ret && cm_statmust(expr5);
	}
	return ret;
}

//////////////////////////////////////////////////////////////////////////

OvBool	statement::expr5::test( CmCompiler* cm )
{
	OvBool ret = cm_statoption(expr4);
	while ( ret && (cm_tokoption('^')) )
	{
		ret = ret && cm_statmust(expr4);
	}
	return ret;
}

//////////////////////////////////////////////////////////////////////////

OvBool	statement::expr4::test( CmCompiler* cm )
{
	OvBool ret = cm_statoption(expr3);
	while ( ret && cm_tokoption('&') )
	{
		if ( cm_tokmatch('&') ) { cm_tokprev(); break; }
		ret = ret && cm_statmust(expr3);
	}
	return ret;
}

//////////////////////////////////////////////////////////////////////////

OvBool	statement::expr3::test( CmCompiler* cm )
{
	OvBool ret = cm_statoption(expr2);
	while ( ret && (cm_tokmatch('>') && cm_lahmatch('>')) || (cm_tokmatch('<') && cm_lahmatch('<')) )
	{
		cm_toknext();cm_toknext();
		ret = ret && cm_statmust(expr2);
	}
	return ret;
}

//////////////////////////////////////////////////////////////////////////

OvBool	statement::expr2::test( CmCompiler* cm )
{
	OvBool ret = cm_statoption(expr1);
	while ( ret && (cm_tokoption('+') || cm_tokoption('-')) )
	{
		ret = ret && cm_statmust(expr1);
	}
	return ret;
}

//////////////////////////////////////////////////////////////////////////

OvBool	statement::expr1::test( CmCompiler* cm )
{
	OvBool ret = cm_statoption(term);
	while ( ret && (cm_tokoption('*') || cm_tokoption('/') || cm_tokoption('%')) )
	{
		ret = ret && cm_statmust(term);
	}
	return ret;
}

//////////////////////////////////////////////////////////////////////////

OvBool	statement::term::test( CmCompiler* cm )
{
	return cm_tokoption(tt_number) || cm_tokoption(tt_identifier) || cm_tokoption(tt_string);
}

//////////////////////////////////////////////////////////////////////////