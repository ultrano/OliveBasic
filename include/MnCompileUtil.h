#pragma once

#define cm_tok				( ( cm->tokpos<cm->tokens.size() )? cm->tokens[cm->tokpos] : CmToken() )
#define cm_lookahead		( ((cm->tokpos+1)<(cm->tokens.size()))? cm->tokens[cm->tokpos+1] : CmToken() )
#define cm_savepos()		( cm->savepos.push_back( cm->tokpos ), (cm->savepos.size()-1) )
#define cm_loadpos(idx)		{ if (idx < cm->savepos.size()) {cm->tokpos = cm->savepos.at(idx);cm->savepos.resize(idx);} }
#define cm_removesave(idx)	{ if (idx < cm->savepos.size()) cm->savepos.resize(idx); }

#define cm_lahmatch(t)		( (cm_lookahead.type) == (t) )
#define cm_tokmatch(t)		( (cm_tok.type) == (t) )
#define cm_tokoption(t)		( cm_tokmatch(t)? (cm_toknext(),true) : false )
#define cm_tokmust(t)		( cm_tokoption(t)? true : (printf( "token: %d, row: %d, col: %d\n", t, cm_tok.row, cm_tok.col ),false) )
#define cm_tokkeyword(kw)	( cm_tokmatch(tt_identifier) && (MnToString(cm_tok.val)->get_str() == kw) )
#define cm_toknext()		( ++cm->tokpos )
#define cm_tokprev()		( --cm->tokpos )

#define cm_getexpr(idx)		( ((idx)>0)? cm->exprs.at( idx - 1 ) : ((idx)<0)? cm->exprs.at( cm->exprs.size() - idx ) : CmExpression() )
#define cm_popexpr(count)	( statement::popexpr(cm,(count)) )
#define cm_pushexpr(expr)	( cm->exprs.push_back( expr ) )
#define cm_pushtemp()		( statement::pushtemp(cm) )

#define cm_addcode(i)		( cm->fi->func->codes.push_back((i)) )
#define cm_addbicode(op)	( statement::addbiop( cm, (op) ) )

#define cm_statmatch(stat)	 (statement::match(cm,&statement::stat::test))
#define cm_statoption(stat)	 (statement::option(cm,&statement::stat::test))
#define cm_statmust(stat)	 (cm_statoption(stat)? true : (printf( "statement: %s, row: %d, col: %d\n", #stat, cm_tok.row, cm_tok.col ),false) )
#define cm_interpret(stat)   ( statement::option(cm,&statement::stat::interpret) )

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
	CmStatements( cm );
}

void CmStatements( CmCompiler* cm )
{
	cm_statoption(multi_stat);
	if ( cm_tokmatch(tt_eos) )
	{
		cm->tokpos = 0;
		cm_interpret(multi_stat);
	}
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

void	statement::pushtemp( CmCompiler* cm )
{
	CmExpression temp;
	temp.type	= et_temp;
	temp.idx	= cm->symbols.size() - cm->level.back() + cm->tempcount;
	++cm->tempcount;
	cm_pushexpr(temp);
}

void	statement::popexpr( CmCompiler* cm, OvSize count )
{
	for ( OvSize i = 0 ; i < count ; ++i )
	{
		CmExpression expr = cm_getexpr( -(i + 1) );
		OvSize symtop = cm->symbols.size() - cm->level.back();
		if ( !cs_isconst(expr.idx) && cs_index(expr.idx) >= symtop ) --cm->tempcount;
		if ( !cs_isconst(expr.extra) && cs_index(expr.extra) >= symtop ) --cm->tempcount;
	}

	cm->tempcount = max(0,cm->tempcount);
	cm->exprs.resize( cm->exprs.size() - count );
}

void statement::addbiop( CmCompiler* cm, opcode op )
{
	CmExpression left	= cm_getexpr(-2);
	CmExpression right	= cm_getexpr(-1);
	cm_popexpr(2);
	cm_pushtemp();
	CmExpression temp	= cm_getexpr(-1);
	
	cm_addcode( cs_code( op, temp.idx, left.idx, right.idx ) );
}

//////////////////////////////////////////////////////////////////////////

OvBool	statement::multi_stat::test( CmCompiler* cm )
{
	while ( cm_statoption(single_stat) );
	return true;
}

OvBool	statement::multi_stat::interpret( CmCompiler* cm )
{
	printf("perfect sentence!!!!!!!!!!!!!!!!!!!!!!!!!\n");
	while ( cm_interpret(single_stat) );
	return true;
}

//////////////////////////////////////////////////////////////////////////

OvBool	statement::single_stat::test( CmCompiler* cm )
{
	if ( cm_statoption(local) ) return true;
	else if ( cm_statoption(block) ) return true;
	else if ( cm_statoption(returnstat) ) return true;
	else if ( cm_statoption(ifstat) ) return true;
	else if ( cm_statoption(whilestat) ) return true;
	else if ( (cm_statoption(expression) && cm_tokmust(';')) ) return true;
	return cm_tokoption(';');
}

OvBool	statement::single_stat::interpret( CmCompiler* cm )
{
	if ( cm_interpret(local) ) return true;
	else if ( cm_interpret(block) ) return true;
	return cm_tokoption(';');
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

OvBool	statement::local::interpret( CmCompiler* cm )
{
	return true;
}

//////////////////////////////////////////////////////////////////////////

OvBool	statement::block::test( CmCompiler* cm )
{
	if ( cm_tokoption('{') )
	{
		cm_statoption(multi_stat);
		return cm_tokmust('}');
	}
	return false;
}

OvBool	statement::block::interpret( CmCompiler* cm )
{
	return cm_tokmatch(tt_identifier);
}

//////////////////////////////////////////////////////////////////////////

OvBool	statement::expression::test( CmCompiler* cm )
{
	return cm_statoption(expr10);
}

OvBool	statement::expression::interpret( CmCompiler* cm )
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

OvBool	statement::expr1::interpret( CmCompiler* cm )
{
	cm_interpret(term);
	return false;
}

//////////////////////////////////////////////////////////////////////////

OvBool	statement::term::test( CmCompiler* cm )
{
	return cm_statoption(postexpr);
}

OvBool	statement::term::interpret( CmCompiler* cm )
{
	return cm_interpret(postexpr);
}

//////////////////////////////////////////////////////////////////////////

OvBool	statement::postexpr::test( CmCompiler* cm )
{
	if ( cm_statoption(primary) )
	{
		while ( true )
		{
			if (cm_statoption(callargs)) continue;
			else if ( cm_tokmatch('.') && cm_lahmatch(tt_identifier) ) {cm_toknext();cm_toknext(); continue; }
			else if ( cm_tokoption('[') )
			{
				cm_statmust(expression);
				cm_tokmust(']');
				continue;
			}
			else break;
		}
		return true;
	}
	return false;
}

OvBool	statement::postexpr::interpret( CmCompiler* cm )
{
	return cm_interpret(primary);
}

//////////////////////////////////////////////////////////////////////////

OvBool	statement::primary::test( CmCompiler* cm )
{
	if ( cm_statoption(funcdesc) ) return true;
	else if ( cm_tokoption(tt_number) ) return true;
	else if ( cm_tokoption(tt_identifier) ) return true;
	else if ( cm_tokoption(tt_string) ) return true;
	else if ( cm_tokoption('(') )
	{
		return cm_statmust(expression) && cm_tokmust(')');
	}
	return false;
}

OvBool	statement::primary::interpret( CmCompiler* cm )
{
	if ( cm_tokoption(tt_number) || cm_tokoption(tt_string) )
	{
		CmExpression expr;
		expr.type = et_const;
		expr.idx  = cs_const( cm->fi->func->consts.size() );
		cm->fi->func->consts.push_back( cm_tok.val );
		cm_pushexpr(expr);
		return true;
	}
	return false;
}

//////////////////////////////////////////////////////////////////////////

OvBool	statement::funcdesc::test( CmCompiler* cm )
{
	if ( cm_tokkeyword("function") )
	{
		cm_toknext();
		cm_tokoption(tt_identifier);
		return cm_statmust(funcargs) && cm_statmust(block);
	}
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
	if (cm_tokoption('('))
	{
		while ( true )
		{
			if (cm_statoption(expression) && cm_tokoption(',')) continue; else break;
		}
		return cm_tokmust(')');
	}
	return false;
}

//////////////////////////////////////////////////////////////////////////

OvBool	statement::returnstat::test( CmCompiler* cm )
{
	if (cm_tokkeyword("return"))
	{
		cm_toknext();
		return cm_statoption(expression) && cm_tokmust(';');
	}
	return false;
}

//////////////////////////////////////////////////////////////////////////

OvBool	statement::ifstat::test( CmCompiler* cm )
{
	if (cm_tokkeyword("if"))
	{
		cm_toknext();
		OvBool ret = true;
		ret = ret && cm_tokmust('(');
		ret = ret && cm_statmust(expression);
		ret = ret && cm_tokmust(')');
		ret = ret && cm_statmust(single_stat);
		return ret;
	}
	return false;
}

//////////////////////////////////////////////////////////////////////////

OvBool	statement::whilestat::test( CmCompiler* cm )
{
	if (cm_tokkeyword("while"))
	{
		cm_toknext();
		OvBool ret = true;
		ret = ret && cm_tokmust('(');
		ret = ret && cm_statmust(expression);
		ret = ret && cm_tokmust(')');
		ret = ret && cm_statmust(single_stat);
		return ret;
	}
	return false;
}

//////////////////////////////////////////////////////////////////////////