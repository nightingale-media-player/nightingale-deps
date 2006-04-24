#include "quicktime.h"



int quicktime_meta_init(quicktime_meta_t *meta)
{
	return 0;
}

int quicktime_meta_delete(quicktime_meta_t *meta)
{
	return 0;
}

int quicktime_meta_dump(quicktime_meta_t *meta)
{
}


int quicktime_read_meta(quicktime_t *file, quicktime_meta_t *meta, quicktime_atom_t *parent_atom)
{
	quicktime_atom_t leaf_atom;

	do
	{
		quicktime_atom_read_header(file, &leaf_atom);
		
		if(quicktime_atom_is(&leaf_atom, "ilst"))
		{
//      quicktime_read_ilst(file, &(meta->ilst));
		}
		else
		if(quicktime_atom_is(&leaf_atom, "iods"))
		{
		}
		else
		{
			quicktime_atom_skip(file, &leaf_atom);
		}
	}while(quicktime_position(file) < parent_atom->end);
	
	return 0;
}

int quicktime_write_meta(quicktime_t *file, quicktime_meta_t *meta)
{
}