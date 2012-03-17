#pragma once

OvInt cp_scaning( MnState* s )
{
	OvFileInputStream fis( mn_tostring(s,1) );
	OvChar c;
	fis.Read(c);
	MnArray* tokarr = ut_newarray(s);
	
	mn_pushstring( s, "tokarr" );
	ut_pushvalue( s, MnValue(MOT_ARRAY,tokarr) );
	mn_setglobal(s);

#define cp_read() (fis.Read(c))
#define cp_settok( tok, n, v ) ut_setfield( s, tok, MnValue( MOT_STRING, ut_newstring(s,n) ), v )
	
	while ( true )
	{
		if ( c == EOF  )
		{
			MnValue tok = MnValue(MOT_TABLE,ut_newtable(s));
			cp_settok( tok, "type", MnValue( (MnNumber)tt_eos ) );
			tokarr->array.push_back( tok );
			return 0;
		}
		else if ( isdigit(c) )
		{
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
				if ( !cp_read() ) break;
			}

			MnValue tok = MnValue(MOT_TABLE,ut_newtable(s));
			cp_settok( tok, "type", MnValue( (MnNumber)tt_number ) );
			cp_settok( tok, "val", MnValue( num ) );
			tokarr->array.push_back( tok );
		}
		else if ( c == '"' )
		{
			OvString str;
			cp_read();
			do
			{
				str.push_back(c);
			} while ( cp_read() && c != '"' );
			cp_read();

			MnValue tok = MnValue(MOT_TABLE,ut_newtable(s));
			cp_settok( tok, "type", MnValue( (MnNumber)tt_string ) );
			cp_settok( tok, "val", MnValue( MOT_STRING, ut_newstring( s, str) ) );
			tokarr->array.push_back( tok );
		}
		else if ( isalpha(c) || c == '_' )
		{
			OvString str;
			do
			{
				str.push_back(c);
			} while ( cp_read() && (isalnum(c) || c == '_') );

			MnValue tok = MnValue(MOT_TABLE,ut_newtable(s));
			cp_settok( tok, "type", MnValue( (MnNumber)tt_identifier ) );
			cp_settok( tok, "val", MnValue( MOT_STRING, ut_newstring( s, str) ) );
			tokarr->array.push_back( tok );
		}
		else if ( isspace( c ) )
		{
			while ( isspace( c ) ) cp_read();
		}
		else
		{
			OvChar ret = c;
			cp_read();
			MnValue tok = MnValue(MOT_TABLE,ut_newtable(s));
			cp_settok( tok, "type", MnValue( (MnNumber)ret ) );
			tokarr->array.push_back( tok );
		}
	}
#undef cp_read
	return 0;
}


void mn_lib_metascript( MnState* s )
{
	mn_pushstring(s,"scaning");
	mn_pushfunction( s, cp_scaning );
	mn_setglobal(s);
}