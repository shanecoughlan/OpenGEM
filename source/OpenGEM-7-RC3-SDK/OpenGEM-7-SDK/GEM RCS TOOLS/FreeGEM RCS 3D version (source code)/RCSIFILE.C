/*************************************************************
 * Copyright 1999 by Caldera Thin Clients, Inc.              *
 * This software is licensed under the GNU Public License.   *
 * Please see LICENSE.TXT for further information.           *
 *************************************************************/
#include "ppdrcs.h"
#include "rcsiext.h"

	VOID
clear_img()
	{
	WORD old_color, old_pen;
	
	old_color = colour;
	old_pen = pen_on;
	colour = WHITE;
	pen_on = ERASEBOX;
	solid_img();
	fb_redraw();
	colour = old_color;
	pen_on = old_pen;
	}
			    

WORD save_icn()
	{
	LPBIT	daddr, maddr;    
	WORD	ret;
					    
	save_fat(FALSE);
	rast_op(3,&hold_area, &hold_mfdb, &hold_area, &save_mfdb);
	if(gl_isicon)
		rast_op(3, &hold_area, &hld2_mfdb, &hold_area, &sav2_mfdb);
	daddr = ad_pbx[DATAWIND].ob_spec; 
	if (gl_isicon)
	{
	/* write data and mask */
		maddr = ad_pbx[MASKWIND].ob_spec;
		ret              = writ_icon(daddr->bi_pdata, gl_wimage, gl_himage, gl_datasize, TRUE);
		if (ret )  return  writ_icon(maddr->bi_pdata, gl_wimage, gl_himage, gl_datasize, FALSE);
		else return( ret );
	}
	else	 
	/* write data only */
		return(	writ_icon(daddr->bi_pdata,
			gl_wimage, gl_himage, gl_datasize, TRUE) );
	}
	


	WORD
svas_icn()
	{
	get_icnfile(&icn_file[0]);
	if(!icn_file [0])
		return(FALSE);
	else		  
		if (save_icn())
			{
			icn_state = FILE_STATE;
			return(TRUE);
			}
		else
			return(FALSE);	
	}   


   	WORD	
new_img()
	{
	WORD	abtn;					  
	if ( icn_edited && !rcs_xpert ){
	 	if ( (abtn = abandon_button()) == 1 ) 
	    		{
			clear_img();				    
			return( TRUE );
			}
		else if (abtn == 2) /* Save Changes */			   
			{
			if ( icn_state == NOFILE_STATE)
				return( svas_icn());
			else
				return(	save_icn());
			}
		else /* cancel */
			return (FALSE);
		}
	else			      
		{
		clear_img();
		return( TRUE );
		}
	}
	
	VOID
do_open()
	{
	    WORD state;
	load_part(save_obj); /*if successfully load, icn_state = file_state*/
	state = icn_state;
	icn_init(FALSE);
	icn_edited = FALSE;
	icn_state = state;	/*icn_init will put icn_state = nofile_state*/
	}
		
	WORD
icnopen()
	{
	WORD	abtn;					  
	if ( icn_edited && !rcs_xpert )
	 	if ( (abtn = abandon_button()) == 1 ) 
	    		{
			do_open();
			return( TRUE );
			}
		else if (abtn == 2) /* Save Changes */			   
			{
			if ( icn_state == NOFILE_STATE)
				if( svas_icn())
					{
					do_open();
					return(TRUE);
					}
				else
					return( FALSE );
			else
				if(save_icn())
					{
					do_open();
					return(TRUE);
					}
				else
					return( FALSE );
	
			}
		else /* cancel */
			return (FALSE) ;
	else			      
		{
		do_open();
		return( TRUE );
		}
	}
	  
	WORD
rvrt_icn()  
	{
	WORD	abtn;					  
	if ( icn_edited && !rcs_xpert )
	 	if ( (abtn = abandon_button()) == 1 ) 
	    		{
			undo_img();				    
			return( TRUE );
			}
		else if (abtn == 2) /* Save Changes */			   
			if ( icn_state == NOFILE_STATE)
				if( svas_icn())
					return(TRUE);
				else
					return(FALSE);
			else
				if(save_icn())
					return(TRUE);
				else
					return(FALSE);
		else /* cancel */
			return (FALSE) ;
	else			      
		{
		undo_img();
		return( TRUE );
		}
	}
