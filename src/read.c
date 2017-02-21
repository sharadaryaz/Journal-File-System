#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "jfs_header.h"
int not_read = 0;

void read_current_value(int data_id, int this_transaction_id)
{
	int new_transaction_id = this_transaction_id;
	int if_committed_flag = 0;
	
		data_version new_version;
		FILE *fp_version4;
		if_committed_flag = if_committed(this_transaction_id);
		while(!if_committed_flag){
			if(new_transaction_id == 100)
			{
				printf("No committed ID's yet\n");
				return;
			}
			new_transaction_id = new_transaction_id-1;
			if_committed_flag = if_committed(new_transaction_id);

		}


		fp_version4 = fopen("version_file6.bin","r+b");
		while(fread(&new_version, sizeof(data_version), 1, fp_version4)==1)
		{
			if(new_version.action_id == new_transaction_id && new_version.data_id == data_id){
				printf("The data in data id %d for action id %d is %s\n",data_id, new_transaction_id, new_version.data );
				not_read = 1;
			
				if(strcmp(new_version.data, "Corrupt_Data") == 0)
				{
				char c;
				printf("\nThe most recent data seems to be corrupted. \nTo recover the original data from Version History, Press R\n");
				scanf("%c",&c);
				scanf("%c",&c);
				if(c == 'r' || c=='R')
					recover(data_id, new_transaction_id);
				}
			}
			
		}

	if(not_read == 0)
		read_current_value(data_id, new_transaction_id-1);
}

int if_committed(int t_id)
{
	FILE *fp_outcome6;
	outcome_record new_outcome;
	fp_outcome6 = fopen("outcome_file9.bin", "rb");
	while(fread(&new_outcome, sizeof(outcome_record), 1, fp_outcome6)==1 )
	{
		if(new_outcome.state == COMMITTED && new_outcome.action_id == t_id)
			return 1;
	}
	return 0;

}

void recover(int data_id, int t_id)
{
	printf("Recovery is going on!\n");
	FILE *fp_write, *fp_commit;
	int found = 0;
	data_version r_version, c_version;
	fp_write = fopen("version_history_file6.bin", "rb");
	printf("Recovering the data!\nData Recovered!");
	long curr_pos;
	curr_pos = ftell(fp_write);
	fseek(fp_write, curr_pos-sizeof(data_version), SEEK_SET);
	while(fread(&r_version, sizeof(data_version), 1, fp_write)==1 && r_version.data_id != data_id)
	{
		printf("Reverse_Version: Checking for data_id %d and %s\n",r_version.data_id, r_version.data );
		curr_pos = curr_pos-sizeof(data_version);
		fseek(fp_write, curr_pos-sizeof(data_version), SEEK_SET);
	}

	c_version = r_version;
	fp_commit = fopen("version_file6.bin","r+b");
		while(fread(&r_version, sizeof(data_version), 1, fp_commit)==1 && !found)
		{
			if(r_version.data_id == data_id && r_version.action_id == t_id)
			{
				curr_pos = ftell(fp_commit);
				fseek(fp_commit, curr_pos-sizeof(data_version), SEEK_SET);
				fwrite(&c_version, sizeof(data_version), 1, fp_commit);
				found = 1;
				fclose(fp_commit);
				break;
			}
		}
		fclose(fp_commit);

	fclose(fp_write);
}

