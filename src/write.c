#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "jfs_header.h"

void write_value(int data_id, int this_transaction_id, int corrupt_flag)
{
	char data[15] ;
	int update = 0;
	long curr_pos;
	char temp[15];

	data_version update_version;
	data_version new_version;
	data_version check_version;

	FILE *fp_version2, *fp_version3;

	printf("\nEnter the value to be written to the file id %d\n",data_id);
	scanf("%s",data);
	if (corrupt_flag == 0)
	{
		strcpy(temp, data);
	}
	else{
		strcpy(temp, "Corrupt_Data");
	}
	
	strcpy(new_version.data, temp);
	new_version.action_id = this_transaction_id;
	new_version.data_id = data_id;

	fp_version2 = fopen("version_file6.bin","r+b");
	while(fread(&update_version, sizeof(data_version), 1, fp_version2)==1 && !update)
	{
		if(update_version.action_id == this_transaction_id && update_version.data_id == data_id)
		{
			curr_pos = ftell(fp_version2);
			fseek(fp_version2, curr_pos-sizeof(data_version), SEEK_SET);
			fwrite(&new_version, sizeof(data_version), 1, fp_version2);
			update = 1;
		}
	}
	fclose(fp_version2);

/****************WRITING TO VERSION HISTORY FOR RECOVERY********************/

	data_version d_version;
	FILE *fp_version;
	fp_version = fopen("version_history_file6.bin","ab");
	d_version.action_id = this_transaction_id;
	d_version.data_id = data_id;
	strcpy(d_version.data, data);
	fwrite(&d_version, sizeof(data_version), 1, fp_version);
	printf("New data written for %d and %d at Version History\n",this_transaction_id, data_id );
	fclose(fp_version);

/********Updating the outcome_record back to PENDING state************/
	
	int update_o = 0;
	long curr_pos_o;

	FILE *fp_outcome5;
	outcome_record update_record;
	outcome_record new_record;

	new_record.action_id = this_transaction_id;
	new_record.state = PENDING;
	new_record.mark_state = MARKED;
	fp_outcome5 = fopen("outcome_file9.bin", "r+b");
	while(fread(&update_record, sizeof(outcome_record),1,fp_outcome5)==1&&!update_o)
	{
		if(update_record.action_id == this_transaction_id){
			curr_pos_o = ftell(fp_outcome5);
			fseek(fp_outcome5, curr_pos_o-sizeof(outcome_record), SEEK_SET);
			fwrite(&new_record, sizeof(outcome_record),1,fp_outcome5);
			update_o = 1;
		}

	}
	fclose(fp_outcome5);

	printf("The data has been succesfully written to %d\n",data_id );

 	/*************TO CHECK IF DATA HAS BEEN WRITTEN TO VERSION FILE************/
 	fp_version3 = fopen("version_file6.bin", "rb");
	while(!feof(fp_version3))
	{

		fread(&check_version, sizeof(data_version), 1, fp_version3);
		if(feof(fp_version3)) 
			break;
		printf("\nThe structure is\nData - %s\nTransaction ID - %d\nData ID - %d\n", check_version.data, check_version.action_id, check_version.data_id);
	
	}

	fclose(fp_version3);

}
