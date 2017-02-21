#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "jfs_header.h"

int count_outcome_record = 0;
int main()
{
	int cti;
	char c;
	int data_arr[4], i = 0; 
	cti = begin_transaction();
		  
	printf("\nYour current Transaction ID is %d\n", cti);	
	recover_fail_stop();
	/*Asking the client to select the data ID's to which it would perform asctions*/
	printf("Please select File ID you would like to change \n1. File ID - 1000\n2. File ID - 1100\n3. File ID - 1200\nPress 0 when you are done!\n");
	do{
		scanf("%d", &data_arr[i++]);
	} while(data_arr[i-1]!=0);
	i = 0;
	/*Creating Versions for all the Data ID's for which changes are going to be made*/
	while(data_arr[i]!=0){
		scanf("%c", &c);
		printf("Press any key to create a new version for %d\n",data_arr[i] );

		if(getchar())	
			new_version(cti, data_arr[i]);
		i++;
	}
	/*Anouncing that the Mark Point has reached and no further versions for any ID's can be created by this Transaction*/
	mark_point_announce(cti);
	/****************DISPLAYING THE MENU FOR FURTHER ACTIONS*******************/
	
	display_menu(data_arr, cti);

}

int new_outcome_record()
{
	outcome_record o_record;
	char str[15], tar[15];
	printf("****Journaling File System****\n");
	FILE *fp , *fp2, *fp_outcome1, *fp_outcome2;
	int current_tran_id;
	/* CHECK IF FILE ALREADY EXIST. IF DOES, COPY THE TRANSACTION ID AND INCREMENT 1
		ELSE CREATE A NEW FILE WITH TRANSACTION ID AS 100*/
	if( access("t_id14.txt", F_OK ) != -1 ) {
		fp = fopen("t_id14.txt", "r+");
    	fgets (str, 15, fp);
    	fclose(fp);
    	fp2 = fopen("t_id14.txt", "w+");
    	current_tran_id = atoi(str);
    	sprintf(tar, "%d", current_tran_id + 1);
    	fputs(tar, fp2);
    	fclose(fp2);
	} 
	else {
    	fp = fopen("t_id14.txt", "w+");
    	current_tran_id = 101;
    	sprintf(str, "%d", current_tran_id);
    	fputs(str, fp);
    	current_tran_id = 100;
    	fclose(fp);
	}

	/*Writing the new Outcome Record*/
	fp_outcome1 = fopen("outcome_file9.bin", "ab");
	o_record.action_id = current_tran_id;
	o_record.state = PENDING;
	o_record.mark_state = UNMARKED;
	fwrite(&o_record, sizeof(outcome_record), 1, fp_outcome1);
	fclose(fp_outcome1);

	/*   To check if the data has been written to the outcome record    */
	fp_outcome2 = fopen("outcome_file9.bin", "rb");


	while(!feof(fp_outcome2))
	{

		fread(&o_record, sizeof(outcome_record), 1, fp_outcome2);
		if(feof(fp_outcome2)) 
			break;
		count_outcome_record++;
		printf("\nThe structure is\nT_ID - %d\nState - %d\nMark_State - %d\n", o_record.action_id, o_record.state, o_record.mark_state);
	
	}

	fclose(fp_outcome2);

	return current_tran_id;

}

void new_version(int current_tran_id, int data_id)
{
	data_version d_version;
	FILE *fp_version;
	fp_version = fopen("version_file6.bin","ab");
	d_version.action_id = current_tran_id;
	d_version.data_id = data_id;
	strcpy(d_version.data ,"NULL");
	fwrite(&d_version, sizeof(data_version), 1, fp_version);
	printf("New version created for %d and %d\n",current_tran_id, data_id );
	fclose(fp_version);

}

void mark_point_announce(int current_tran_id)
{
	int update = 0;
	long curr_pos;

	FILE *fp_outcome3;
	outcome_record update_record;
	outcome_record new_record;

	new_record.action_id = current_tran_id;
	new_record.state = PENDING;
	new_record.mark_state = MARKED;
	fp_outcome3 = fopen("outcome_file9.bin", "r+b");
	while(fread(&update_record, sizeof(outcome_record),1,fp_outcome3)==1&&!update)
	{
		if(update_record.action_id == current_tran_id){
			curr_pos = ftell(fp_outcome3);
			fseek(fp_outcome3, curr_pos-sizeof(outcome_record), SEEK_SET);
			fwrite(&new_record, sizeof(outcome_record),1,fp_outcome3);
			update = 1;
		}

	}
	fclose(fp_outcome3);
	printf("\nEverything's Marked\n");
}


void display_menu(int data_arr[4], int cti)
{
	int choice=0, flag = 0;
	int data_id,i;
	int corrupt_flag = 0;
	while(choice!=6)
	{
		printf("\nPlease select one of the following options!\n1. Read from the file\n2. Write to the file\n3. Commit all the values\n4. Fail-Fast Write\n5. Fail-Stop Commit\n6. Abort\n");
		scanf("%d",&choice);
		switch(choice)
		{
			case 2: flag = 0;
					corrupt_flag = 0;
					while(flag!=1)
					{
						printf("Enter the File ID you would like to change\n");
						scanf("%d",&data_id);
						for(i=0;i<4;i++)
							if(data_arr[i] == data_id)
								flag = 1;
						if(flag == 0) 
							printf("\nPlease enter the ID that you have decleared to change!\n");
					}
					write_value(data_id, cti,corrupt_flag);
					//flag_write++;
					
					break;

			case 3: commit(cti);
					break;
			case 1:  flag = 0;
					while(flag!=1)
					{
						printf("Enter the File ID you would like to read from\n");
						scanf("%d",&data_id);
						for(i=0;i<4;i++)
							if(data_id == 1100||data_id == 1000||data_id == 1200)
								flag = 1;
						if(flag == 0) 
							printf("\nPlease enter the ID among 1000, 1100, 1200!\n");
					}
					read_current_value(data_id, cti);
					break;
			case 4: flag = 0;
					while(flag!=1)
					{
						printf("Enter the File ID you would like to change\n");
						scanf("%d",&data_id);
						for(i=0;i<4;i++)
							if(data_arr[i] == data_id)
								flag = 1;
						if(flag == 0) 
							printf("\nPlease enter the ID that you have decleared to change!\n");
					}
					corrupt_flag = 1;
					write_value(data_id, cti, corrupt_flag);
					break;
			case 5: commit_fail_stop(cti);
					printf("The program aborted due to unknown reasons\n");
					exit(1);
					break;

		}
	}
}


void commit(int this_transaction_id)
{
	int update = 0;
	long curr_pos;

	FILE *fp_outcome5;
	outcome_record update_record;
	outcome_record new_record;

	new_record.action_id = this_transaction_id;
	new_record.state = COMMITTED;
	new_record.mark_state = MARKED;
	fp_outcome5 = fopen("outcome_file9.bin", "r+b");
	while(fread(&update_record, sizeof(outcome_record),1,fp_outcome5)==1&&!update)
	{
		if(update_record.action_id == this_transaction_id){
			curr_pos = ftell(fp_outcome5);
			fseek(fp_outcome5, curr_pos-sizeof(outcome_record), SEEK_SET);
			fwrite(&new_record, sizeof(outcome_record),1,fp_outcome5);
			update = 1;
		}

	}
	fclose(fp_outcome5);
	printf("\nTransaction ID %d has been succesfully committed!\n", this_transaction_id);
}


void commit_fail_stop(int cti)
{
	FILE *fp_cff;
	int current_tran_id;
	char str[15];
	fp_cff = fopen("recovery1.txt", "w+");
	current_tran_id = cti;
	sprintf(str, "%d", current_tran_id);
	fputs(str, fp_cff);
	fclose(fp_cff);
}


void recover_fail_stop()
{
	if(access("recovery1.txt", F_OK) != -1)
	{
	FILE *fp_cff;
	int cti, zero = 0;
	char str[15], tar[15], c;
	sprintf(tar, "%d", zero);
	/*Opening File to see if recovery is needed*/
	fp_cff = fopen("recovery1.txt","r+");
	fgets(str, 15, fp_cff);
	fputs(tar, fp_cff);
	fclose(fp_cff);

	fp_cff = fopen("recovery1.txt","w+");
	fputs(tar, fp_cff);
	fclose(fp_cff);

	cti = atoi(str);

	if(cti>100)
	{
		printf("The program unexpectedly aborted last time\n");
		printf("The Outcome Record for %d was not committed. Press Y to Commit it.\nPress N to Proceed without committing. Data to %d would be lost\n", cti, cti);
		scanf("%c",&c);
		scanf("%c",&c);
		if(c )
		{
			int update = 0;
			long curr_pos;

			FILE *fp_outcome6;
			outcome_record update_record;
			outcome_record new_record;

			new_record.action_id = cti;
			new_record.state = COMMITTED;
			new_record.mark_state = MARKED;
			fp_outcome6 = fopen("outcome_file9.bin", "r+b");
			while(fread(&update_record, sizeof(outcome_record),1,fp_outcome6)==1&&!update)
			{
				if(update_record.action_id == cti){
					curr_pos = ftell(fp_outcome6);
					fseek(fp_outcome6, curr_pos-sizeof(outcome_record), SEEK_SET);
					fwrite(&new_record, sizeof(outcome_record),1,fp_outcome6);
					update = 1;
				}

			}
			fclose(fp_outcome6);
			printf("\nTransaction ID %d has been succesfully committed!\n", cti);
		}
	}
	}
}




