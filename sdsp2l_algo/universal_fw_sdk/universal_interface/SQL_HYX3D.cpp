#include <assert.h>
#include <stdio.h>      /* printf, scanf, puts, NULL */
#include <stdlib.h>     /* srand, rand */
#include <string.h>
#include "NewSQLCmd.h"
////////////////////////////Hynix 3D////////////////////////
HYX3D_SQLCmd::HYX3D_SQLCmd(FH_Paramater *fh)
{
	// Parsing flash ID
	_fh = fh;
}

HYX3D_SQLCmd::~HYX3D_SQLCmd()
{
}

int HYX3D_SQLCmd::GetOTP( unsigned char ce, unsigned char die, bool bSLCMode, unsigned char *buf )
{
	int ret;
	unsigned int Count=0;
	const unsigned char hynix3D = _IsHynix3D(_fh);
	unsigned char sql[256],slen=0,temp;
	_scsi->Log(LOGDEBUG,"%s:\n", __FUNCTION__);
	
	sql[slen]=0xCA;slen++;	// Stare from 0xCA
	sql[slen]=0x00;slen++;	// temp set Command len = 0
	temp=0x01;				// default used one addrress mode

	sql[slen]=temp;slen++;	// Operation setting
	sql[slen]=0xC2;slen++;	
	sql[slen]=0xFF;slen++;	// reset flash
	sql[slen]=0xCB;slen++;	// wait Ready

	sql[slen]=0xC2;slen++;
	sql[slen]=0x78;slen++;
	sql[slen]=0xAB;slen++;
	sql[slen]=0x00;slen++;
	sql[slen]=0xAB;slen++;
	sql[slen]=0x00;slen++;
	sql[slen]=0xAB;slen++;
	sql[slen]=die;slen++;

	sql[slen]=0xC2;slen++;
	sql[slen]=0x17;slen++;
	sql[slen]=0xC2;slen++;
	sql[slen]=0x04;slen++;
	sql[slen]=0xC2;slen++;
	sql[slen]=0x19;slen++;
	sql[slen]=0xC2;slen++;
	sql[slen]=0xDA;slen++;

	sql[slen]=0xC2;slen++;
	sql[slen]=0x00;slen++;
	sql[slen]=0xAB;slen++;
	sql[slen]=0x00;slen++;
	sql[slen]=0xAB;slen++;
	sql[slen]=0x00;slen++;
	sql[slen]=0xC2;slen++;
	if ( !_fh->beD3)
	{
		if( hynix3D == 2 )
		{
			if( bSLCMode )// 3dv2  flash slc mode
			{
				Count = 816;
				sql[slen]=0x5A;slen++;
				//FREG_FIP[FCTL_PIO_ADDR] = 0x3E;	//Hubert 180523, from 0x6E to 0x3E
			}
			else
			{
				Count = 5616;
				sql[slen]=0x46;slen++;
				//FREG_FIP[FCTL_PIO_ADDR] = 0x2E;	//Hubert 180523, from 0x5E to 0x2E
			}
		}
	}
	else
	{
		if( hynix3D == 3 )
		{
			if( bSLCMode )// 3dv3  flash slc mode
			{
				Count = 816;
				sql[slen]=0x3E;slen++;
				//FREG_FIP[FCTL_PIO_ADDR] = 0x3E;	//Hubert 180523, from 0x6E to 0x3E
			}
			else
			{
				Count = 5616;
				sql[slen]=0x2E;slen++;
				//FREG_FIP[FCTL_PIO_ADDR] = 0x2E;	//Hubert 180523, from 0x5E to 0x2E
			}
		}
		else if( hynix3D == 4 )	// 3dv4 flash 
		{
			if( bSLCMode )  //slc mode
			{
				Count = 816;
				//FREG_FIP[FCTL_PIO_ADDR] = 0xAE;	//for channel SSD
				//FREG_FIP[FCTL_PIO_ADDR] = 0xBE;	//for channel card
				sql[slen]=0xBE;slen++;
			}
			else
			{
				Count = 5616;
				//FREG_FIP[FCTL_PIO_ADDR] = 0xAD;	//for channel SSD
				//FREG_FIP[FCTL_PIO_ADDR] = 0xBD;	//for channel card
				sql[slen]=0xBD;slen++;
			}
		}
		else if( hynix3D == 5 )	// 3dv5 flash 
		{
			if( bSLCMode )//16G ed3 flash slc mode
			{
				Count = 816;
				//FREG_FIP[FCTL_PIO_ADDR] = 0x24;	//for channel SSD
				//FREG_FIP[FCTL_PIO_ADDR] = 0x22;	//for channel card
				sql[slen]=0xFA;slen++;
			}
			else
			{
				Count = 5616;
				//FREG_FIP[FCTL_PIO_ADDR] = 0x04;	//for channel SSD
				//FREG_FIP[FCTL_PIO_ADDR] = 0x02;	//for channel card
				sql[slen]=0xF9;slen++;
			}
		}
		else
		{
			assert(0);
		}
	}
	
	sql[slen]=0xAB;slen++;
	sql[slen]=0x00;slen++;
	sql[slen]=0xAB;slen++;
	sql[slen]=die;slen++;
	sql[slen]=0xC2;slen++;
	sql[slen]=0x30;slen++;
	sql[slen]=0xCB;slen++;
	sql[slen]=0xDA;slen++;
	sql[slen]=(Count+1023)/1024;slen++;
	sql[slen]=0xC2;slen++;
	sql[slen]=0xFF;slen++;
	sql[slen]=0xCB;slen++;

	sql[slen]=0xCE;slen++;			// command End
	sql[1]=slen-3;
	sql[slen]=0xA0;slen++;

	SetFTA(&sql[slen], 0, 0, 0);
	slen+=6;
	
	sql[slen]=0xAE;slen++;
	sql[slen]=0xAF;slen++;

    //int ret=_scsi->Dev_IssueIOCmd(sql, slen, 0, ce, 0, 0);
    ret=Dev_IssueIOCmd(sql, slen, 2, ce, buf, 0);
	return ret;
}

int HYX3D_SQLCmd::SetRetryValue( unsigned char ce, bool bSLCMode, int len, unsigned char *value, unsigned int block )
{
	assert(0);
	return 0;
}

int HYX3D_SQLCmd::PlaneEraseD1( unsigned char ce, unsigned int block, unsigned char planemode )
{
	int ret;
	_scsi->Log(LOGDEBUG,"%s:\n", __FUNCTION__);
	//Erase( ce, block, bSLCMode, planemode, bDFmode, bWaitRDY, bAddr6 );
	ret = Erase( ce, block, true, planemode, _bDFmode, _bWaitRDY, _bAddr6 );
	return ret;
}

int HYX3D_SQLCmd::PlaneErase( unsigned char ce, unsigned int block, unsigned char planemode )
{
	int ret;
	_scsi->Log(LOGDEBUG,"%s:\n", __FUNCTION__);
	//Erase( ce, block, bSLCMode, planemode, bDFmode, bWaitRDY, bAddr6 );
	ret = Erase( ce, block, false, planemode, _bDFmode, _bWaitRDY, _bAddr6 );
	return ret;
}

int HYX3D_SQLCmd::DumpWriteD1( unsigned char ce, unsigned int block, unsigned int page, unsigned int pagecnt, const unsigned char* buf, int len, unsigned char planemode, bool bCache )
{
	int ret = 0;
	unsigned int blk, pg;
	bool bLastPage = false;

	_scsi->Log(LOGDEBUG,"%s:\n", __FUNCTION__);
	if( ((page+pagecnt)>_fh->MaxPage) || (!pagecnt) )
	{
		pagecnt = (_fh->MaxPage-page);
	}
	for( pg=0; pg<pagecnt; pg++ )
	{
		if( (bCache) && (pg==(pagecnt-1)) && (pagecnt!=1) )
		{
			bLastPage = true;
		}
		else if( pg==(_fh->MaxPage-1) )
		{
			bLastPage = true;
		}
		for( blk=0; blk < planemode; blk++ )
		{
			//Write( ce, block, page, bConvs, bSLCMode, buf, len, planemode, ProgStep, bCache, bLastPage, bWaitRDY, bAddr6);
			ret = Write( ce, (block+blk), (page+pg), false, true, &buf[pg*len], len, planemode, 0, bCache, bLastPage, true, _bAddr6  );
			if(ret)
			{
				return ret;
			}
		}
	}
	return ret;
}

int HYX3D_SQLCmd::DumpWrite( unsigned char ce, unsigned int block, unsigned int page, unsigned int pagecnt, const unsigned char* buf, int len, unsigned char planemode, bool bCache )
{
	int ret = 0;
	unsigned int blk, pg, WL;
	bool bLastPage = false;
	unsigned char p_cnt, ProgStep = 0;
	unsigned char cell = LogTwo(_fh->Cell);

	_scsi->Log(LOGDEBUG,"%s:\n", __FUNCTION__);
	if( ((page+pagecnt)>(_fh->MaxPage*cell)) || (!pagecnt) )
	{
		pagecnt = ((_fh->MaxPage*cell)-page);
	}
	for( pg = 0; pg < pagecnt; pg++ )
	{
		WL = ((page+pg)/cell);
		p_cnt = ((page+pg)%cell);
		if( (bCache) && (pg==(pagecnt-1)) && (pagecnt!=1) )
		{
			bLastPage = true;
		}
		else if( pg==((_fh->MaxPage*cell)-1) )
		{
			bLastPage = true;
		}
		for( blk=0; blk < planemode; blk++ )
		{
			//Write( ce, block, page, bConvs, bSLCMode, buf, len, planemode, ProgStep, bCache, bLastPage, bWaitRDY, bAddr6 );
			ret = Write( ce, (block+blk), WL, false, false, &buf[pg*len], len, planemode, ((p_cnt<<4)|ProgStep), bCache, bLastPage, true, _bAddr6 );
			if(ret)
			{
				return ret;
			}
		}
	}
	return ret;
}

int HYX3D_SQLCmd::GroupWriteD1( unsigned char ce, unsigned int block, unsigned int page, unsigned int pagecnt, unsigned char* buf, int len, unsigned char patternmode, unsigned char planemode, bool bCache )
{
	int v, i, ret = 0;
	unsigned int blk, pg;
	bool bLastPage = false;

	_scsi->Log(LOGDEBUG,"%s:\n", __FUNCTION__);
	if( ((page+pagecnt)>_fh->MaxPage) || (!pagecnt) )
	{
		pagecnt = (_fh->MaxPage-page);
	}
	if(patternmode==0) 
	{
		for( i=0; i<len; i++  )
		{
		    buf[i] = (rand()&0xFF);
		}
	}
	else if(patternmode==1)
	{
		memset( buf, 0, len );
		v=0;
		for ( i=0; i<len; i+=2 ) 
		{
			buf[i] = (v&0xFF);
			v++;
		}
	}
	else if(patternmode==2) // used select pattern
	{	
		//memset(buf,0,len);
	}
	else if(patternmode==3)
	{
        	memset( buf, 0xff, len );
	}
	else {
		assert(0);
	}

	for( pg=0; pg<pagecnt; pg++ )
	{
		int block_seed = _scsi->GetSelectBlockSeed();
		int ldpc_mode = _scsi->GetSelectLDPC_mode();
		_scsi->Dev_SQL_GroupWR_Setting(block_seed, pg, true, ldpc_mode, 'C');
		if( (bCache) && (pg==(pagecnt-1)) )
		{
			bLastPage = true;
		}
		for( blk=0; blk<planemode; blk++ )
		{
			if( (patternmode==0) || (patternmode==1) ) 
			{
				for( i=0; i<(len-4); i+=256 ) 
				{
					buf[i] = (block&0xff);
					buf[i+1] = ((block>>8)&0xff);
					buf[i+2] = ((page+pg)&0xff);
					buf[i+3] = (((page+pg)>>8)&0xff);
				}
			}
			//Write(ce, block, page, bConvs, bSLCMode, buf, len, planemode, ProgStep, bCache, bLastPage, bWaitRDY, bAddr6);
			if( patternmode==2 )
			{
				ret = Write( ce, block+blk, page+pg, true, true, &buf[pg*len], len, planemode, 0, bCache, bLastPage, true, _bAddr6 );
			}
			else
			{
				ret = Write( ce, block+blk, page+pg, true, true, buf, len, planemode, 0, bCache, bLastPage, true, _bAddr6 );
			}
			if(ret)
			{
				return ret;
			}
		}
	}
	return ret;
}

int HYX3D_SQLCmd::GroupWrite( unsigned char ce, unsigned int block, unsigned int page, unsigned int pagecnt, unsigned char* buf, int len, unsigned char patternmode, unsigned char planemode, bool bCache )
{
	int v, i, ret = 0;
	unsigned int blk, pg, WL;
	bool bLastPage = false;
	unsigned char p_cnt, ProgStep=0;
	unsigned char cell = LogTwo(_fh->Cell);

	_scsi->Log(LOGDEBUG,"%s:\n", __FUNCTION__);
	if( ((page+pagecnt)>(_fh->MaxPage*cell)) || (!pagecnt) )
	{
		pagecnt = ((_fh->MaxPage*cell)-page);
	}
	if(patternmode==0) 
	{
		for( i=0; i<len; i++  )
		{
		    buf[i] = (rand()&0xFF);
		}
	}
	else if(patternmode==1)
	{
		memset( buf, 0, len );
		v=0;
		for ( i=0; i<len; i+=2 ) 
		{
			buf[i] = (v&0xFF);
			v++;
		}
	}
    else if(patternmode==2) // used select pattern
	{	
        //memset(buf,0,len);
    }
    else if(patternmode==3)
	{
        memset( buf, 0xff, len );
    }		

	for( pg=0; pg<pagecnt; pg++ )
	{
		int block_seed = _scsi->GetSelectBlockSeed();
		int ldpc_mode = _scsi->GetSelectLDPC_mode();
		_scsi->Dev_SQL_GroupWR_Setting(block_seed, pg, true, ldpc_mode, 'C');
		WL = ((page+pg)/cell);
		p_cnt = ((page+pg)%cell);
		if( (bCache) && (pg==(pagecnt-1)) )
		{
			bLastPage = true;
		}
		for( blk=0; blk<planemode; blk++ )
		{
			if( (patternmode==0) || (patternmode==1) ) 
			{
				for( i=0; i<(len-4); i+=256 ) 
				{
					buf[i] = (block&0xff);
					buf[i+1] = ((block>>8)&0xff);
					buf[i+2] = ((page+pg)&0xff);
					buf[i+3] = (((page+pg)>>8)&0xff);
				}
			}
			//Write(ce, block, page, bConvs, bSLCMode, buf, len, planemode, ProgStep, bCache, bLastPage, bWaitRDY, bAddr6);
			if( patternmode==2 )
			{
				ret = Write( ce, block+blk, WL, true, false, &buf[pg*len], len, planemode, ((p_cnt<<4)|ProgStep), bCache, bLastPage, true, _bAddr6 );
			}
			else
			{
				ret = Write( ce, block+blk, WL, true, false, buf, len, planemode, ((p_cnt<<4)|ProgStep), bCache, bLastPage, true, _bAddr6 );
			}
			if(ret)
			{
				return ret;
			}
		}
	}
	return ret;
}

int HYX3D_SQLCmd::DumpReadD1( unsigned char ce, unsigned int block, unsigned int page, unsigned int pagecnt, unsigned char* buf, int len, unsigned char planemode, bool bCache, bool bRetry )
{
	int ret = 0;
	unsigned int blk, pg;
	bool bLastPage = false;

	_scsi->Log(LOGDEBUG,"%s:\n", __FUNCTION__);
	if( ((page+pagecnt)>_fh->MaxPage) || (!pagecnt) )
	{
		pagecnt = (_fh->MaxPage-page);
	}
	
	if( (planemode==1) && (!bCache) )
	{
		for( pg = 0; pg < pagecnt; pg++ )
		{
			//Read( ce, block, page, bConvs, bSLCMode, buf, len, planemode, bReadCmd, bReadData, bReadEcc, bCache, bFirstPage, bLastPage, bCmd36, bWaitRDY, bAddr6);
			ret = Read( ce, block, (page+pg), false, true, &buf[pg*len], len, planemode, true, true, false, false, false, false, false, true, _bAddr6 );
			if(ret)
			{
				return ret;
			}
		}
	}
	else if( (!bCache) )	// Plane mode != 1
	{
		for( pg = 0; pg < pagecnt; pg++ )
		{
			for( blk=0; blk < planemode; blk++ )
			{
				//Read( ce, block, page, bConvs, bSLCMode, buf, len, planemode, bReadCmd, bReadData, bReadEcc, bCache, bFirstPage, bLastPage, bCmd36, bWaitRDY, bAddr6);
				ret = Read( ce, (block+blk), (page+pg), false, true, buf, len, planemode, true, false, false, false, false, false, false, true, _bAddr6 ); // send 
				if(ret)
				{
					return ret;
				}
			}
			for( blk=0; blk < planemode; blk++ )
			{
				//Read( ce, block, page, bConvs, bSLCMode, buf, len, planemode, bReadCmd, bReadData, bReadEcc, bCache, bFirstPage, bLastPage, bCmd36, bWaitRDY, bAddr6);
				ret = Read( ce, (block+blk), (page+pg), false, true, &buf[((blk*pagecnt)+pg)*len], len, planemode, false, true, false, false, false, false, false, true, _bAddr6 );
				if(ret)
				{
					return ret;
				}
			}
		}
	}
	else
	{
		for( pg = 0; pg < pagecnt; pg++ )
		{
			for( blk=0; blk < planemode; blk++ )
			{
				//send command only
				//Read( ce, block, page, bConvs, bSLCMode, buf, len, planemode, bReadCmd, bReadData, bReadEcc, bCache, bFirstPage, bLastPage, bCmd36, bWaitRDY, bAddr6);
				ret = Read( ce, (block+blk), (page+pg), false, true, buf, len, planemode, true, false, false, true, (pg==0), false, false, true, _bAddr6 );
				if(ret)
				{
					return ret;
				}
			}
			if(pg!=0)
			{
				//read data
				for( blk=0; blk < planemode; blk++ )
				{
					//Read( ce, block, page, bConvs, bSLCMode, buf, len, planemode, bReadCmd, bReadData, bReadEcc, bCache, bFirstPage, bLastPage, bCmd36, bWaitRDY, bAddr6);
					ret = Read( ce, (block+blk), (page+(pg-1)), false, true, &buf[((blk*pagecnt)+(pg-1))*len], len, planemode, false, true, false, true, false, false, false, true, _bAddr6 );
					if(ret)
					{
						return ret;
					}
				}
			}
		}
		// for cache last page 
		for( blk=0; blk < planemode; blk++ )
		{
			//Read( ce, block, page, bConvs, bSLCMode, buf, len, planemode, bReadCmd, bReadData, bReadEcc, bCache, bFirstPage, bLastPage, bCmd36, bWaitRDY, bAddr6);
			ret = Read( ce, (block+blk), (page+(pagecnt-1)), false, true, buf, len, planemode, true, false, false, true, false, true, false, true, _bAddr6 );
			if(ret)
			{
				return ret;
			}
		}
		//read data
		for( blk=0; blk < planemode; blk++ )
		{
			//Read( ce, block, page, bConvs, bSLCMode, buf, len, planemode, bReadCmd, bReadData, bReadEcc, bCache, bFirstPage, bLastPage, bCmd36, bWaitRDY, bAddr6);
			ret = Read( ce, (block+blk), (page+(pagecnt-1)), false, true, &buf[((blk*pagecnt)+(pg-1))*len], len, planemode, false, true, false, true, false, true, false, true, _bAddr6 );
			if(ret)
			{
				return ret;
			}
		}
	}
	return ret;
}

int HYX3D_SQLCmd::DumpRead( unsigned char ce, unsigned int block, unsigned int page, unsigned int pagecnt, unsigned char* buf, int len, unsigned char planemode, bool bCache, bool bRetry )
{
	int ret = 0;
	unsigned int blk, pg;
	bool bLastPage = false;
	unsigned char cell = LogTwo(_fh->Cell);

	_scsi->Log(LOGDEBUG,"%s:\n", __FUNCTION__);
	if( ((page+pagecnt)>(_fh->MaxPage*cell)) || (!pagecnt) )
	{
		pagecnt = ((_fh->MaxPage*cell)-page);
	}
	
	if( (planemode==1) && (!bCache) )
	{
		for( pg = 0; pg < pagecnt; pg++ )
		{
			//Read( ce, block, page, bConvs, bSLCMode, buf, len, planemode, bReadCmd, bReadData, bReadEcc, bCache, bFirstPage, bLastPage, bCmd36, bWaitRDY, bAddr6);
			ret = Read( ce, block, (page+pg), false, false, &buf[pg*len], len, planemode, true, true, false, false, false, false, false, true, _bAddr6 );
			if(ret)
			{
				return ret;
			}
		}
	}
	else if( (!bCache) )	// Plane mode != 1
	{
		for( pg = 0; pg < pagecnt; pg++ )
		{
			for( blk=0; blk < planemode; blk++ )
			{
				//Read( ce, block, page, bConvs, bSLCMode, buf, len, planemode, bReadCmd, bReadData, bReadEcc, bCache, bFirstPage, bLastPage, bCmd36, bWaitRDY, bAddr6);
				ret = Read( ce, (block+blk), (page+pg), false, false, buf, len, planemode, true, false, false, false, false, false, false, true, _bAddr6 ); // send
				if(ret)
				{
					return ret;
				}
			}
			for( blk=0; blk < planemode; blk++ )
			{
				//Read( ce, block, page, bConvs, bSLCMode, buf, len, planemode, bReadCmd, bReadData, bReadEcc, bCache, bFirstPage, bLastPage, bCmd36, bWaitRDY, bAddr6);
				ret = Read( ce, (block+blk), (page+pg), false, false, &buf[((blk*pagecnt)+pg)*len], len, planemode, false, true, false, false, false, false, false, true, _bAddr6 );
				if(ret)
				{
					return ret;
				}
			}
		}
	}
	else
	{
		for( pg = 0; pg < pagecnt; pg++ )
		{
			for( blk=0; blk < planemode; blk++ )
			{
				//send command only
				//Read( ce, block, page, bConvs, bSLCMode, buf, len, planemode, bReadCmd, bReadData, bReadEcc, bCache, bFirstPage, bLastPage, bCmd36, bWaitRDY, bAddr6);
				ret = Read( ce, (block+blk), (page+pg), false, false, buf, len, planemode, true, false, false, true, (pg==0), false, false, true, _bAddr6 );
				if(ret)
				{
					return ret;
				}
			}
			if(pg!=0)
			{
				//read data
				for( blk=0; blk < planemode; blk++ )
				{
					//Read( ce, block, page, bConvs, bSLCMode, buf, len, planemode, bReadCmd, bReadData, bReadEcc, bCache, bFirstPage, bLastPage, bCmd36, bWaitRDY, bAddr6);
					ret = Read( ce, (block+blk), (page+(pg-1)), false, false, &buf[((blk*pagecnt)+(pg-1))*len], len, planemode, false, true, false, true, false, false, false, true, _bAddr6 );
					if(ret)
					{
						return ret;
					}
				}
			}
		}
		// for cache last page 
		for( blk=0; blk < planemode; blk++ )
		{
			//Read( ce, block, page, bConvs, bSLCMode, buf, len, planemode, bReadCmd, bReadData, bReadEcc, bCache, bFirstPage, bLastPage, bCmd36, bWaitRDY, bAddr6);
			ret = Read( ce, (block+blk), (page+(pagecnt-1)), false, false, buf, len, planemode, true, false, false, true, false, true, false, true, _bAddr6 );
			if(ret)
			{
				return ret;
			}
		}
		//read data
		for( blk=0; blk < planemode; blk++ )
		{
			//Read( ce, block, page, bConvs, bSLCMode, buf, len, planemode, bReadCmd, bReadData, bReadEcc, bCache, bFirstPage, bLastPage, bCmd36, bWaitRDY, bAddr6);
			ret = Read( ce, (block+blk), (page+(pagecnt-1)), false, false, &buf[((blk*pagecnt)+(pg-1))*len], len, planemode, false, true, false, true, false, true, false, true, _bAddr6 );
			if(ret)
			{
				return ret;
			}
		}
	}
	return ret;
}

int HYX3D_SQLCmd::GroupReadDataD1( unsigned char ce, unsigned int block, unsigned int page, unsigned int pagecnt, unsigned char* buf, int len, unsigned char planemode, bool bCache, bool bRetry )
{
	int ret = 0;
	unsigned int blk, pg;
	bool bLastPage = false;

	_scsi->Log(LOGDEBUG,"%s:\n", __FUNCTION__);
	if( ((page+pagecnt)>_fh->MaxPage) || (!pagecnt) )
	{
		pagecnt = (_fh->MaxPage-page);
	}
	
	if( (planemode==1) && (!bCache) )
	{
		for( pg = 0; pg < pagecnt; pg++ )
		{
			//Read( ce, block, page, bConvs, bSLCMode, buf, len, planemode, bReadCmd, bReadData, bReadEcc, bCache, bFirstPage, bLastPage, bCmd36, bWaitRDY, bAddr6);
			ret = Read( ce, block, (page+pg), true, true, &buf[pg*len], len, planemode, true, true, false, false, false, false, false, true, _bAddr6 );
			if(ret)
			{
				return ret;
			}
		}
	}
	else if( (!bCache) )	// Plane mode != 1
	{
		for( pg = 0; pg < pagecnt; pg++ )
		{
			for( blk=0; blk < planemode; blk++ )
			{
				//Read( ce, block, page, bConvs, bSLCMode, buf, len, planemode, bReadCmd, bReadData, bReadEcc, bCache, bFirstPage, bLastPage, bCmd36, bWaitRDY, bAddr6);
				ret = Read( ce, (block+blk), (page+pg), true, true, buf, len, planemode, true, false, false, false, false, false, false, true, _bAddr6 ); // send 
				if(ret)
				{
					return ret;
				}
			}
			for( blk=0; blk < planemode; blk++ )
			{
				//Read( ce, block, page, bConvs, bSLCMode, buf, len, planemode, bReadCmd, bReadData, bReadEcc, bCache, bFirstPage, bLastPage, bCmd36, bWaitRDY, bAddr6);
				ret = Read( ce, (block+blk), (page+pg), true, true, &buf[((blk*pagecnt)+pg)*len], len, planemode, false, true, false, false, false, false, false, true, _bAddr6 );
				if(ret)
				{
					return ret;
				}
			}
		}
	}
	else
	{
		for( pg = 0; pg < pagecnt; pg++ )
		{
			for( blk=0; blk < planemode; blk++ )
			{
				//send command only
				//Read( ce, block, page, bConvs, bSLCMode, buf, len, planemode, bReadCmd, bReadData, bReadEcc, bCache, bFirstPage, bLastPage, bCmd36, bWaitRDY, bAddr6);
				ret = Read( ce, (block+blk), (page+pg), true, true, buf, len, planemode, true, false, false, true, (pg==0), false, false, true, _bAddr6 );
				if(ret)
				{
					return ret;
				}
			}
			if(pg!=0)
			{
				//read data
				for( blk=0; blk < planemode; blk++ )
				{
					//Read( ce, block, page, bConvs, bSLCMode, buf, len, planemode, bReadCmd, bReadData, bReadEcc, bCache, bFirstPage, bLastPage, bCmd36, bWaitRDY, bAddr6);
					ret = Read( ce, (block+blk), (page+(pg-1)), true, true, &buf[((blk*pagecnt)+(pg-1))*len], len, planemode, false, true, false, true, false, false, false, true, _bAddr6 );
					if(ret)
					{
						return ret;
					}
				}
			}
		}
		// for cache last page 
		for( blk=0; blk < planemode; blk++ )
		{
			//Read( ce, block, page, bConvs, bSLCMode, buf, len, planemode, bReadCmd, bReadData, bReadEcc, bCache, bFirstPage, bLastPage, bCmd36, bWaitRDY, bAddr6);
			ret = Read( ce, (block+blk), (page+(pagecnt-1)), true, true, buf, len, planemode, true, false, false, true, false, true, false, true, _bAddr6 );
			if(ret)
			{
				return ret;
			}
		}
		//read data
		for( blk=0; blk < planemode; blk++ )
		{
			//Read( ce, block, page, bConvs, bSLCMode, buf, len, planemode, bReadCmd, bReadData, bReadEcc, bCache, bFirstPage, bLastPage, bCmd36, bWaitRDY, bAddr6);
			ret = Read( ce, (block+blk), (page+(pagecnt-1)), true, true, &buf[((blk*pagecnt)+(pg-1))*len], len, planemode, false, true, false, true, false, true, false, true, _bAddr6 );
			if(ret)
			{
				return ret;
			}
		}
	}
	return ret;
}

int HYX3D_SQLCmd::GroupReadData( unsigned char ce, unsigned int block, unsigned int page, unsigned int pagecnt, unsigned char* buf, int len, unsigned char planemode, bool bCache, bool bRetry )
{
	int ret = 0;
	unsigned int blk, pg;
	bool bLastPage = false;
	unsigned char cell = LogTwo(_fh->Cell);

	_scsi->Log(LOGDEBUG,"%s:\n", __FUNCTION__);
	if( ((page+pagecnt)>(_fh->MaxPage*cell)) || (!pagecnt) )
	{
		pagecnt = ((_fh->MaxPage*cell)-page);
	}
	
	if( (planemode==1) && (!bCache) )
	{
		for( pg = 0; pg < pagecnt; pg++ )
		{
			//Read( ce, block, page, bConvs, bSLCMode, buf, len, planemode, bReadCmd, bReadData, bReadEcc, bCache, bFirstPage, bLastPage, bCmd36, bWaitRDY, bAddr6);
			ret = Read( ce, block, (page+pg), true, false, &buf[pg*len], len, planemode, true, true, false, false, false, false, false, true, _bAddr6 );
			if(ret)
			{
				return ret;
			}
		}
	}
	else if( (!bCache) )	// Plane mode != 1
	{
		for( pg = 0; pg < pagecnt; pg++ )
		{
			for( blk=0; blk < planemode; blk++ )
			{
				//Read( ce, block, page, bConvs, bSLCMode, buf, len, planemode, bReadCmd, bReadData, bReadEcc, bCache, bFirstPage, bLastPage, bCmd36, bWaitRDY, bAddr6);
				ret = Read( ce, (block+blk), (page+pg), true, false, buf, len, planemode, true, false, false, false, false, false, false, true, _bAddr6 ); // send 
				if(ret)
				{
					return ret;
				}
			}
			for( blk=0; blk < planemode; blk++ )
			{
				//Read( ce, block, page, bConvs, bSLCMode, buf, len, planemode, bReadCmd, bReadData, bReadEcc, bCache, bFirstPage, bLastPage, bCmd36, bWaitRDY, bAddr6);
				ret = Read( ce, (block+blk), (page+pg), true, false, &buf[((blk*pagecnt)+pg)*len], len, planemode, false, true, false, false, false, false, false, true, _bAddr6 );
				if(ret)
				{
					return ret;
				}
			}
		}
	}
	else
	{
		for( pg = 0; pg < pagecnt; pg++ )
		{
			for( blk=0; blk < planemode; blk++ )
			{
				//send command only
				//Read( ce, block, page, bConvs, bSLCMode, buf, len, planemode, bReadCmd, bReadData, bReadEcc, bCache, bFirstPage, bLastPage, bCmd36, bWaitRDY, bAddr6);
				ret = Read( ce, (block+blk), (page+pg), true, false, buf, len, planemode, true, false, false, true, (pg==0), false, false, true, _bAddr6 );
				if(ret)
				{
					return ret;
				}
			}
			if(pg!=0)
			{
				//read data
				for( blk=0; blk < planemode; blk++ )
				{
					//Read( ce, block, page, bConvs, bSLCMode, buf, len, planemode, bReadCmd, bReadData, bReadEcc, bCache, bFirstPage, bLastPage, bCmd36, bWaitRDY, bAddr6);
					ret = Read( ce, (block+blk), (page+(pg-1)), true, false, &buf[((blk*pagecnt)+(pg-1))*len], len, planemode, false, true, false, true, false, false, false, true, _bAddr6 );
					if(ret)
					{
						return ret;
					}
				}
			}
		}
		// for cache last page 
		for( blk=0; blk < planemode; blk++ )
		{
			//Read( ce, block, page, bConvs, bSLCMode, buf, len, planemode, bReadCmd, bReadData, bReadEcc, bCache, bFirstPage, bLastPage, bCmd36, bWaitRDY, bAddr6);
			ret = Read( ce, (block+blk), (page+(pagecnt-1)), true, false, buf, len, planemode, true, false, false, true, false, true, false, true, _bAddr6 );
			if(ret)
			{
				return ret;
			}
		}
		//read data
		for( blk=0; blk < planemode; blk++ )
		{
			//Read( ce, block, page, bConvs, bSLCMode, buf, len, planemode, bReadCmd, bReadData, bReadEcc, bCache, bFirstPage, bLastPage, bCmd36, bWaitRDY, bAddr6);
			ret = Read( ce, (block+blk), (page+(pagecnt-1)), true, false, &buf[((blk*pagecnt)+(pg-1))*len], len, planemode, false, true, false, true, false, true, false, true, _bAddr6 );
			if(ret)
			{
				return ret;
			}
		}
	}
	return ret;
}

int HYX3D_SQLCmd::GroupReadEccD1( unsigned char ce, unsigned int block, unsigned int page, unsigned int pagecnt, unsigned char* buf, int len, unsigned char planemode, bool bCache, bool bRetry )
{
	int ret = 0;
	unsigned int blk, pg;
	bool bLastPage = false;

	_scsi->Log(LOGDEBUG,"%s:\n", __FUNCTION__);
	if( ((page+pagecnt)>_fh->MaxPage) || (!pagecnt) )
	{
		pagecnt = (_fh->MaxPage-page);
	}
	
	if( (planemode==1) && (!bCache) )
	{
		for( pg = 0; pg < pagecnt; pg++ )
		{
			//Read( ce, block, page, bConvs, bSLCMode, buf, len, planemode, bReadCmd, bReadData, bReadEcc, bCache, bFirstPage, bLastPage, bCmd36, bWaitRDY, bAddr6);
			ret = Read( ce, block, (page+pg), true, true, &buf[pg*len], len, planemode, true, true, true, false, false, false, false, true, _bAddr6 );
			if(ret)
			{
				return ret;
			}
		}
	}
	else if( (!bCache) )	// Plane mode != 1
	{
		for( pg = 0; pg < pagecnt; pg++ )
		{
			for( blk=0; blk < planemode; blk++ )
			{
				//Read( ce, block, page, bConvs, bSLCMode, buf, len, planemode, bReadCmd, bReadData, bReadEcc, bCache, bFirstPage, bLastPage, bCmd36, bWaitRDY, bAddr6);
				ret = Read( ce, (block+blk), (page+pg), true, true, buf, len, planemode, true, false, true, false, false, false, false, true, _bAddr6 ); // send 
				if(ret)
				{
					return ret;
				}
			}
			for( blk=0; blk < planemode; blk++ )
			{
				//Read( ce, block, page, bConvs, bSLCMode, buf, len, planemode, bReadCmd, bReadData, bReadEcc, bCache, bFirstPage, bLastPage, bCmd36, bWaitRDY, bAddr6);
				ret = Read( ce, (block+blk), (page+pg), true, true, &buf[((blk*pagecnt)+pg)*len], len, planemode, false, true, true, false, false, false, false, true, _bAddr6 );
				if(ret)
				{
					return ret;
				}
			}
		}
	}
	else
	{
		for( pg = 0; pg < pagecnt; pg++ )
		{
			for( blk=0; blk < planemode; blk++ )
			{
				//send command only
				//Read( ce, block, page, bConvs, bSLCMode, buf, len, planemode, bReadCmd, bReadData, bReadEcc, bCache, bFirstPage, bLastPage, bCmd36, bWaitRDY, bAddr6);
				ret = Read( ce, (block+blk), (page+pg), true, true, buf, len, planemode, true, false, true, true, (pg==0), false, false, true, _bAddr6 );
				if(ret)
				{
					return ret;
				}
			}
			if(pg!=0)
			{
				//read data
				for( blk=0; blk < planemode; blk++ )
				{
					//Read( ce, block, page, bConvs, bSLCMode, buf, len, planemode, bReadCmd, bReadData, bReadEcc, bCache, bFirstPage, bLastPage, bCmd36, bWaitRDY, bAddr6);
					ret = Read( ce, (block+blk), (page+(pg-1)), true, true, &buf[((blk*pagecnt)+(pg-1))*len], len, planemode, false, true, true, true, false, false, false, true, _bAddr6 );
					if(ret)
					{
						return ret;
					}
				}
			}
		}
		// for cache last page 
		for( blk=0; blk < planemode; blk++ )
		{
			//Read( ce, block, page, bConvs, bSLCMode, buf, len, planemode, bReadCmd, bReadData, bReadEcc, bCache, bFirstPage, bLastPage, bCmd36, bWaitRDY, bAddr6);
			ret = Read( ce, (block+blk), (page+(pagecnt-1)), true, true, buf, len, planemode, true, false, true, true, false, true, false, true, _bAddr6 );
			if(ret)
			{
				return ret;
			}
		}
		//read data
		for( blk=0; blk < planemode; blk++ )
		{
			//Read( ce, block, page, bConvs, bSLCMode, buf, len, planemode, bReadCmd, bReadData, bReadEcc, bCache, bFirstPage, bLastPage, bCmd36, bWaitRDY, bAddr6);
			ret = Read( ce, (block+blk), (page+(pagecnt-1)), true, true, &buf[((blk*pagecnt)+(pg-1))*len], len, planemode, false, true, true, true, false, true, false, true, _bAddr6 );
			if(ret)
			{
				return ret;
			}
		}
	}
	return ret;
}

int HYX3D_SQLCmd::GroupReadEcc( unsigned char ce, unsigned int block, unsigned int page, unsigned int pagecnt, unsigned char* buf, int len, unsigned char planemode, bool bCache, bool bRetry )
{
	int ret = 0;
	unsigned int blk, pg;
	bool bLastPage = false;
	unsigned char cell = LogTwo(_fh->Cell);

	_scsi->Log(LOGDEBUG,"%s:\n", __FUNCTION__);
	if( ((page+pagecnt)>(_fh->MaxPage*cell)) || (!pagecnt) )
	{
		pagecnt = ((_fh->MaxPage*cell)-page);
	}
	
	if( (planemode==1) && (!bCache) )
	{
		for( pg = 0; pg < pagecnt; pg++ )
		{
			//Read( ce, block, page, bConvs, bSLCMode, buf, len, planemode, bReadCmd, bReadData, bReadEcc, bCache, bFirstPage, bLastPage, bCmd36, bWaitRDY, bAddr6);
			ret = Read( ce, block, (page+pg), true, false, &buf[pg*len], len, planemode, true, true, true, false, false, false, false, true, _bAddr6 );
			if(ret)
			{
				return ret;
			}
		}
	}
	else if( (!bCache) )	// Plane mode != 1
	{
		for( pg = 0; pg < pagecnt; pg++ )
		{
			for( blk=0; blk < planemode; blk++ )
			{
				//Read( ce, block, page, bConvs, bSLCMode, buf, len, planemode, bReadCmd, bReadData, bReadEcc, bCache, bFirstPage, bLastPage, bCmd36, bWaitRDY, bAddr6);
				ret = Read( ce, (block+blk), (page+pg), true, false, buf, len, planemode, true, false, true, false, false, false, false, true, _bAddr6 ); // send 
				if(ret)
				{
					return ret;
				}
			}
			for( blk=0; blk < planemode; blk++ )
			{
				//Read( ce, block, page, bConvs, bSLCMode, buf, len, planemode, bReadCmd, bReadData, bReadEcc, bCache, bFirstPage, bLastPage, bCmd36, bWaitRDY, bAddr6);
				ret = Read( ce, (block+blk), (page+pg), true, false, &buf[((blk*pagecnt)+pg)*len], len, planemode, false, true, true, false, false, false, false, true, _bAddr6 );
				if(ret)
				{
					return ret;
				}
			}
		}
	}
	else
	{
		for( pg = 0; pg < pagecnt; pg++ )
		{
			for( blk=0; blk < planemode; blk++ )
			{
				//send command only
				//Read( ce, block, page, bConvs, bSLCMode, buf, len, planemode, bReadCmd, bReadData, bReadEcc, bCache, bFirstPage, bLastPage, bCmd36, bWaitRDY, bAddr6);
				ret = Read( ce, (block+blk), (page+pg), true, false, buf, len, planemode, true, false, true, true, (pg==0), false, false, true, _bAddr6 );
				if(ret)
				{
					return ret;
				}
			}
			if(pg!=0)
			{
				//read data
				for( blk=0; blk < planemode; blk++ )
				{
					//Read( ce, block, page, bConvs, bSLCMode, buf, len, planemode, bReadCmd, bReadData, bReadEcc, bCache, bFirstPage, bLastPage, bCmd36, bWaitRDY, bAddr6);
					ret = Read( ce, (block+blk), (page+(pg-1)), true, false, &buf[((blk*pagecnt)+(pg-1))*len], len, planemode, false, true, true, true, false, false, false, true, _bAddr6 );
					if(ret)
					{
						return ret;
					}
				}
			}
		}
		// for cache last page 
		for( blk=0; blk < planemode; blk++ )
		{
			//Read( ce, block, page, bConvs, bSLCMode, buf, len, planemode, bReadCmd, bReadData, bReadEcc, bCache, bFirstPage, bLastPage, bCmd36, bWaitRDY, bAddr6);
			ret = Read( ce, (block+blk), (page+(pagecnt-1)), true, false, buf, len, planemode, true, false, true, true, false, true, false, true, _bAddr6 );
			if(ret)
			{
				return ret;
			}
		}
		//read data
		for( blk=0; blk < planemode; blk++ )
		{
			//Read( ce, block, page, bConvs, bSLCMode, buf, len, planemode, bReadCmd, bReadData, bReadEcc, bCache, bFirstPage, bLastPage, bCmd36, bWaitRDY, bAddr6);
			ret = Read( ce, (block+blk), (page+(pagecnt-1)), true, false, &buf[((blk*pagecnt)+(pg-1))*len], len, planemode, false, true, true, true, false, true, false, true, _bAddr6 );
			if(ret)
			{
				return ret;
			}
		}
	}
	return ret;
}