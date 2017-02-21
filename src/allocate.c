#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "jfs_header.h"

FILE *fp_outcome4;
int begin_transaction()
{
	int id, flag = 0;
	int previous_id;
	//char c;
	id = new_outcome_record();
	printf("New id is %d\n",id );
	if(id == 100)
		return id;
	previous_id = id - 1;
	flag = check_for_marked(previous_id);
	fclose(fp_outcome4);
	if(flag == 1)
		return id;
	else return 0;

}

int check_for_marked(int prev_id)
{

	outcome_record o_come;
	

	fp_outcome4 = fopen("outcome_file9.bin", "rb");
	while(!feof(fp_outcome4))
	{
		fread(&o_come, sizeof(outcome_record), 1, fp_outcome4);
		if(o_come.action_id == prev_id)
		{
			if(o_come.mark_state == UNMARKED ){
				//return 0;
				//scanf("%c", &c);
				
				printf("\nThe previous ID has not been marked yet.\nIf you would like to retry, press Y" );
				if(getchar())
				check_for_marked(prev_id);

			}
			//else return 1;

		}

	}	
	return 1;
}
