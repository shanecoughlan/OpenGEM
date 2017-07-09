

#include "ppdgem.h"
#include "ppdg0.h"


	WORD
graf_mouse(m_number, m_addr)
	WORD		m_number;
	LPUWORD		m_addr;
{
	GR_MNUMBER = m_number;
	GR_MADDR = m_addr;
	return( gem_if( GRAF_MOUSE ) );
}
