/*	RCSFILES.C	10/08/84 - 1/25/85	Tim Oren		*/
/*************************************************************
 * Copyright 1999 by Caldera Thin Clients, Inc.              *
 * This software is licensed under the GNU Public License.   *
 * Please see LICENSE.TXT for further information.           *
 *************************************************************/
#include "ppdrcs.h"


	VOID
get_path(tmp_path, spec)
	BYTE	*tmp_path, *spec;
	{
	WORD	cur_drv;

	cur_drv = dos_gdrv();
	tmp_path[0] = cur_drv + 'A';
	tmp_path[1] = ':';
	tmp_path[2] = '\\';
	dos_gdir(cur_drv+1, ADDR(&tmp_path[3]));
	if (strlen(tmp_path) > 3) strcat(tmp_path, "\\");
	else tmp_path[2] = '\0';
	strcat(tmp_path, spec);
	return;
	}

VOID r_to_xfile(BYTE *where, BYTE *ext)
	{
	WORD	ii, jj;

	strcpy( where, rcs_rfile );

	ii = (jj = strlen(where)) - 1 ; 

	while ((ii && where[ii] != '.') && where[ii] != '\\') ii-- ;
	if (where[ii] != '.')
		where[ii = jj] = '.';
	strcpy( &where[++ii], ext);
 		
	}

WORD get_file()
	{
	WORD	butn, ii;
	BYTE	tmp_path[64], tmp_name[13];
		
	if ( !sav_rcspath[0] ) get_defpath(tmp_path, "*.RSC");
	else                   strcpy(tmp_path, sav_rcspath);
	tmp_name[0] = '\0';

	fsel_input(ADDR(&tmp_path[0]), ADDR(&tmp_name[0]), &butn);
	graf_mouse(rcs_mform, 0x0L);
	if (butn)
		{
		if (!tmp_name[0])
		   if (hndl_alert(1, string_addr(NOFNAME)) == 1) return(FALSE);
		if (!strcmp(sav_rcspath, tmp_path))
			{
			dos_chdir(ADDR(tmp_path));
			strcpy(sav_rcspath, tmp_path);
			}
		strcpy(rcs_rfile, &tmp_path[0]);
	 	for (ii = 0; rcs_rfile[ii] && rcs_rfile[ii] != '*'; ii++);
	  	rcs_rfile[ii - 1] = 0;	/* knock off slash */
		strcat(rcs_rfile, "\\");
		strcpy(&rsc_path[1], rcs_rfile);
		rsc_path[0] = strlen(rcs_rfile);
	  	strcat(rcs_rfile, &tmp_name[0]);
		/* Force 'rsc' extension on resource file.*/
		r_to_xfile(rcs_rfile, "RSC"); 
		r_to_xfile(rcs_dfile, "DFN");
		return (TRUE);
		} /* if butn */
	else
		return (FALSE);
	} /* get_file */

	WORD
rvrt_files()
	{
	if (!open_files(TRUE))
		return(FALSE);
	if (!read_files())
		return(FALSE);
	redo_trees();
	return(TRUE);
	}	

	WORD 
ok_rvrt_files()   /* kluge routine to allow calls between sibling overlays */
	{
	if (rvrt_files() )
		{
		new_state( FILE_STATE );
		rsc_title(); 
		}
	else
		view_trees();
	return( cont_rcsinit(FALSE)); /* return to rcs_init in overlay */		      
	}

VOID rsc_title()
	{
     	set_title(&rcs_rfile[0]);
	}

	VOID
clr_rsc()
	{
	ini_buff();
	rcs_ndxno = 0;
	new_state(NOFILE_STATE);
	clr_title();
	rcs_trpan = 0;
	redo_trees();
	}
				
	WORD
abandon_button()
	{		
	return(  hndl_alert(1,string_addr(ABNDCHGS)) );
	}
	       

	VOID
cls_rsc()
	{
	mouse_form(HGLASS);
	if (write_files())
		clr_rsc();
	mouse_form(ARROW);
	}
	
	
	WORD
new_rsc()						      
	{
	WORD	abtn;
	if ( rcs_edited && !rcs_xpert )
	 	if ( (abtn = abandon_button()) == 1 ) 
	    		{
			clr_rsc();				    
			return( TRUE );
			}
		else if (abtn == 2) /* Save Changes */			   
			{
			if ( rcs_state == NOFILE_STATE)
				if (!get_file())
					return(FALSE);
			cls_rsc();
			clr_rsc();
			return(TRUE);
			}
		else /* cancel */
			return (FALSE) ;
	else			      
		{
		clr_rsc();
		return( FALSE );
		}
	}

	WORD
opn_rsc()
	{
	mouse_form(HGLASS);
	if (open_files(FALSE))
		{
		if (read_files())
			{  
			new_state(FILE_STATE);
			rsc_title();
			}
		}
	else 
		{
		mouse_form(ARROW);
		return(FALSE);	   
		}
	rcs_trpan = 0;
	redo_trees();
	mouse_form(ARROW);
	return(TRUE);
	}

	WORD
open_rsc()
	{
	WORD	abtn;
	if (rcs_edited && !rcs_xpert)
		{
		if ((abtn = abandon_button()) == 1)
			return(opn_rsc());
		else if (abtn == 2)
			{	  
			if (rcs_state == NOFILE_STATE)
				if( ! get_file() )
					return(FALSE);
			cls_rsc();
			return(opn_rsc());
			}
		else
			return(FALSE);
		}
	else			     
		 return (opn_rsc());
	}
   
	WORD
clos_rsc()
	{
	WORD	abtn;
	if (rcs_edited && !rcs_xpert)
		if ((abtn = abandon_button()) == 1)
			{
			clr_rsc();
			return(TRUE);
			}
		else if (abtn == 2)
			{	  
			if ( rcs_state == NOFILE_STATE)
				if (!get_file())
					return(FALSE);
			cls_rsc();
			return(TRUE);
			}
		else
			return(FALSE);
	else			     
		{
		clr_rsc();
		return(TRUE);
		}
	}

	
	VOID
save_rsc()
	{
	mouse_form(HGLASS);
	write_files();
	rvrt_files();
	mouse_form(ARROW);
	}

	WORD
svas_rsc()
	{

	if (!get_file())
		return(FALSE);
	mouse_form(HGLASS);
	write_files();
	rvrt_files();
	mouse_form(ARROW);
	new_state(FILE_STATE);
	rsc_title();	      
	return(TRUE);
	}

	WORD
rvrt_rsc()
	{
	WORD	abtn;
	if (rcs_edited && !rcs_xpert)
		if ((abtn = abandon_button()) == 1)
			{
			mouse_form(HGLASS);
			if ( rcs_state == NOFILE_STATE)
				clr_rsc();
			else	 
				rvrt_files();
			mouse_form(ARROW);
			return(TRUE);
			}
		else if (abtn == 2)	     
			{
			if( rcs_state == NOFILE_STATE)
				if( ! get_file() )
					return(FALSE);
			cls_rsc();
			return(TRUE);
			}
		else
			return(FALSE);
	else
		{
		mouse_form(HGLASS);
		rvrt_files();
		mouse_form(ARROW);   
		return(FALSE);
		}	
	}

	VOID
merge_rsc()
	{
	BYTE	sv_rfile[80], sv_dfile[80];

	strcpy(sv_rfile, rcs_rfile);
	strcpy(sv_dfile, rcs_dfile);
	mouse_form(HGLASS);
	if (open_files(FALSE))
	if (merge_files())  redo_trees();
	if (rcs_state == FILE_STATE)
		{
		strcpy(rcs_rfile, sv_rfile);
		strcpy(rcs_dfile, sv_dfile);
		}
	mouse_form(ARROW);
	}
	
	WORD
quit_rsc()						      
	{
	WORD	abtn;
	if ( rcs_edited && !rcs_xpert )
	 	if ( (abtn = abandon_button()) == 1 ) 
    			return(TRUE);
		else if (abtn == 2) /* Save Changes */			   
			{
			if( rcs_state == NOFILE_STATE)
				if( ! get_file() )
					return(FALSE);
			cls_rsc();
			return(TRUE);
			}
		else /* cancel */
			return (FALSE);
	else
		return(TRUE);
	}
