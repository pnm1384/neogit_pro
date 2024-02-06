#include<stdio.h>
#include<stdlib.h>
#include<dirent.h>
#include<string.h>
#include<unistd.h>
#include<stdbool.h>
#include<sys/stat.h>
#include<sys/types.h>
#include<time.h>

char repo_add[1000], all_repo_files[2000][2000], copied_file_array[2000][2000];
char Month_names[12][4] = {"Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"};
int copied_file_array_ok[2000];
char temppp[1000];
int DDDD, number_of_all_repo_files, size_of_copied_file_array;
FILE *PTR_global;
bool Flag;
char **argv2;

/*
   commit file shape:
   id
   branch
   parent id
   date and time
   author name
   author email
   commit message
   number of files
*/

/*
	PROBLEMS:
	when detecting deleted file in committing, delete it ?
*/

void run_reset_undo();

int get_parent_of_commit();

void remove_the_enter(char *ss)
{
	int nnnn = strlen(ss);
	if (nnnn == 0) return;
	if (ss[nnnn - 1] == '\n') ss[nnnn - 1] = '\0';
	return;
}

int is_white(char cc)
{
	if (cc == ' ' || cc == '\n' || cc == '\t') return 1;
	return 0;
}

int get_number_of_month(char *ss)
{
	for (int i = 0; i < 12; i++)
	{
		if (strcmp(ss, Month_names[i]) == 0) return i + 1;
	}
	return -1;
}

void print_one_file_info(char *add)
{
	FILE *ptr = fopen(add, "r");
	char ssss[1000];
	while (fgets(ssss, sizeof(ssss), ptr) != NULL)
	{
		printf("%s", ssss);
	}
	fclose(ptr);
	return;
}

int have_line_in_file(char *add, char *ss)
{
	FILE *ptr = fopen(add, "r");
	bool have_it = false;
	char ssss[1000];
	while (fgets(ssss, sizeof(ssss), ptr) != NULL)
	{
		remove_the_enter(ssss);
		if (strcmp(ssss, ss) == 0)
		{
			have_it = true;
			break;
		}
	}
	fclose(ptr);
	if (have_it) return 1;
	return 0;
}

int get_number_of_lines_bad(char *add)
{
	int ttt = 0;
	FILE *ptr = fopen(add, "r");
	char ssss[1000];
	while (fgets(ssss, sizeof(ssss), ptr) != NULL)
	{
		ttt++;
	}
	fclose(ptr);
	return ttt;
}

void add_line_to_file(char *add, char *ss)
{
	FILE *ptr = fopen(add, "a");
	fprintf(ptr, "%s\n", ss);
	fclose(ptr);
	return;
}

void remove_line_from_file(char *add, char *ss)  // not checked yet !
{
	FILE *ptr = fopen(add, "r");
	char ssss[1000];
	int ttt = 0;
	while (fgets(ssss, sizeof(ssss), ptr) != NULL)
	{
		remove_the_enter(ssss);
		if (strcmp(ssss, ss) == 0) break;
		ttt++;
	}
	fclose(ptr);
	char add2[1000];
	strcpy(add2, add);
	int indx = -1;
	int nnn = strlen(add2);
	for (int i = nnn - 1; i >= 0; i--)
	{
		if (add2[i] == '/')
		{
			indx = i;
			break;
		}
	}
	strcpy(add2 + indx + 1, "tmp_file");
	ptr = fopen(add, "r");
	FILE *ptr2 = fopen(add2, "w");
	int ttt2 = 0;
	while (fgets(ssss, sizeof(ssss), ptr) != NULL)
	{
		if (ttt != ttt2)
		{
			fprintf(ptr2, "%s", ssss);
		}
		ttt2++;
	}
	fclose(ptr); fclose(ptr2);
	remove(add);
	rename(add2, add);
	return;
}

void get_file_format_bad(char *ss, char *add) // receives address
{
	ss[0] = '\0';
	int nnnn = strlen(add);
	int indx = -1;
	for (int i = nnnn - 1; i >= 0; i--)
	{
		if (add[i] == '#')
		{
			indx = i;
			break;
		}
	}
	for (int i = indx - 1; i >= 0; i--)
	{
		if (add[i] == '#')
		{
			return;
		}
		if (add[i] == '.')
		{
			int xxx = 0;
			for (int j = i; j < indx; j++) ss[xxx++] = add[j];
			ss[xxx] = '\0';
			return;
		}
	}
	return;
}

int date_bigger_than_date(int d1, int m1, int y1, int d2, int m2, int y2)
{
	if (y1 != y2)
	{
		if (y1 > y2) return 1;
		return 0;
	}
	if (m1 != m2)
	{
		if (m1 > m2) return 1;
		return 0;
	}
	if (d1 >= d2) return 1;
	return 0;
}

void change_add_to_name(char *ss)
{
	int nnn = strlen(ss);
	for (int i = 0; i < nnn; i++)
	{
		if (ss[i] == '/') ss[i] = '#';
	}
	return;
}

void copy_file_into_array(char *add) // not checked yet !!
{
	size_of_copied_file_array = 0;
	char ssss[1000];
	FILE *ptr = fopen(add, "r");
	while (fgets(ssss, sizeof(ssss), ptr) != NULL)
	{
		remove_the_enter(ssss);
		strcpy(copied_file_array[size_of_copied_file_array++], ssss);
	}
	fclose(ptr);
	return;
}

int remove_num_from_name(char *ss)
{
	int nnnn = strlen(ss);
	int indx = -1;
	for (int i = nnnn - 1; i >= 0; i--)
	{
		if (ss[i] == '#')
		{
			indx = i;
			break;
		}
	}
	ss[indx] = '\0';
	return indx;
}

int get_number_of_lines(char *add) // maybe empty (just for checking equality)
{
	int ttt = 0;
	FILE *ptr = fopen(add, "r");
	char ssss[1000];
	while (fgets(ssss, sizeof(ssss), ptr) != NULL) ttt++;
	fclose(ptr);
	return ttt;
}

void shift_to_right(char *argv[], int ind, int sz) // does not change the size !
{
	for (int i = sz - 1; i >= ind; i--) strcpy(argv[i + 1], argv[i]);
	return;
}

void shift_to_left(char *argv[], int ind, int sz) // does not change the size !
{
	for (int i = ind; i < sz - 1; i++) strcpy(argv[i], argv[i + 1]);
	return;
}

int files_are_equal(char *add1, char *add2) // not checked yet !
{
	int n1 = get_number_of_lines(add1), n2 = get_number_of_lines(add2);
	if (n1 != n2) return 0;
	char ssss1[1000], ssss2[1000];
	FILE *ptr1 = fopen(add1, "r"), *ptr2 = fopen(add2, "r");
	bool barba = true;
	while (fgets(ssss1, sizeof(ssss1), ptr1) != NULL)
	{
		fgets(ssss2, sizeof(ssss2), ptr2);
		if (strcmp(ssss1, ssss2) != 0)
		{
			barba = false;
			break;
		}
	}
	fclose(ptr1); fclose(ptr2);
	if (barba) return 1;
	return 0;
}

void get_repo_add()
{
	repo_add[0] = '\0';
	char now_address[1000];
	getcwd(now_address, sizeof(now_address));
	struct dirent *entry;
	char tmp_add[1000];
	bool vojood = false;
	while (true)
	{
		DIR *dir = opendir(".");
		while ((entry = readdir(dir)) != NULL)
		{
			if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) continue;
			if (entry->d_type == DT_DIR && strcmp(entry->d_name, ".neogit") == 0)
			{
				getcwd(repo_add, sizeof(repo_add));
				vojood = true;
				break;
			}
		}
		closedir(dir);
		if (vojood) break;
		getcwd(tmp_add, sizeof(tmp_add));
		if (strcmp(tmp_add, "/") == 0) break;
		chdir("..");
	}
	chdir(now_address);
	return;
}

void empty_reset_history()
{
	get_repo_add();
	char tmpp[1000];
	strcpy(tmpp, repo_add);
	strcat(tmpp, "/.neogit/reset_history");
	FILE *ptr = fopen(tmpp, "w");
	fclose(ptr);
	return;
}

int get_add_history_cnt() // not checked yet !
{
	get_repo_add();
	char tmpp[1000], ssss[1000];
	strcpy(tmpp, repo_add);
	strcat(tmpp, "/.neogit/add_history_cnt");
	FILE *ptr = fopen(tmpp, "r");
	fgets(ssss, sizeof(ssss), ptr);
	remove_the_enter(ssss);
	int ans = -1;
	sscanf(ssss, "%d", &ans);
	fclose(ptr);
	return ans;
}

void empty_add_history() // and cnt = 0
{
	get_repo_add();
	char tmpp[1000];
	int cnt_have = get_add_history_cnt();
	strcpy(tmpp, repo_add);
	strcat(tmpp, "/.neogit/add_history_cnt");
	FILE *ptr = fopen(tmpp, "w");
	fprintf(ptr, "0");
	fclose(ptr);
	for (int i = 1; i <= cnt_have; i++)
	{
		strcpy(tmpp, repo_add);
		strcat(tmpp, "/.neogit/add_history/");
		int xxx = i;
		int nnnn = strlen(tmpp);
		while (xxx > 0)
		{
			tmpp[nnnn++] = xxx % 10 + '0';
			xxx /= 10;
		}
		tmpp[nnnn] = '\0';
		remove(tmpp);
	}
	return;
}

void get_all_repo_files_in_folder(char *add) // get_all_repo_files() checked one time
{
	char tmpp[1000];
	DIR *dir = opendir(add);
	struct dirent *entry;
	while ((entry = readdir(dir)) != NULL)
	{
		if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0 || strcmp(entry->d_name, ".neogit") == 0) continue;
		if (entry->d_type == DT_REG)
		{
			strcpy(tmpp, add);
			strcat(tmpp, "/");
			strcat(tmpp, entry->d_name);
			strcpy(all_repo_files[number_of_all_repo_files++], tmpp);
			continue;
		}
		if (entry->d_type == DT_DIR)
		{
			strcpy(tmpp, add);
			strcat(tmpp, "/");
			strcat(tmpp, entry->d_name);
			get_all_repo_files_in_folder(tmpp);
		}
	}
	closedir(dir);
	return;
}

void get_all_repo_files() // checked one time
{
	get_repo_add();
	number_of_all_repo_files = 0;
	get_all_repo_files_in_folder(repo_add);
	return;
}

int file_really_exists(char *add) // receives address of file  // not checked yet !
{
	int indx = -1;
	int nnnn = strlen(add);
	for (int i = nnnn - 1; i >= 0; i--)
	{
		if (add[i] == '/')
		{
			indx = i;
			break;
		}
	}
	add[indx] = '\0'; // remember to return it !!!! //
	char sssss[1000];
	strcpy(sssss, add + indx + 1);
	DIR *dir = opendir(add);
	struct dirent *entry;
	bool have_it = false;
	while ((entry = readdir(dir)) != NULL)
	{
		if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0 || strcmp(entry->d_name, ".neogit") == 0) continue;
		// check the type if file ???? //
		if (strcmp(entry->d_name, sssss) == 0)
		{
			have_it = true;
			break;
		}
	}
	closedir(dir);
	add[indx] = '/';
	if (have_it) return 1;
	return 0;
}

void change_name_to_add(char *ss)
{
	int nnn = strlen(ss);
	for (int i = 0; i < nnn; i++)
	{
		if (ss[i] == '#') ss[i] = '/';
	}
	return;
}

void get_the_time(char *ss) // not checked yet !
{
	time_t cur_time;
	time(&cur_time);
	char *ffat = ctime(&cur_time);
	int nnnn = strlen(ffat);
	if (ffat[nnnn - 1] == '\n') ffat[nnnn - 1] = '\0';
	strcpy(ss, ffat);
	return;
}

void get_author_name(char *ss) // not checked yet !
{
	get_repo_add();
	FILE *ptr = fopen("/home/parsa/.neogit_prop/repo_adds", "r");
	int ttt = 0;
	char ssss[1000];
	while (fgets(ssss, sizeof(ssss), ptr) != NULL)
	{
		int nnnn = strlen(ssss);
		if (ssss[nnnn - 1] == '\n') ssss[nnnn - 1] = '\0';
		if (strcmp(ssss, repo_add) == 0) break;
		ttt++;
	}
	fclose(ptr);
	int ttt2 = 0;
	int stat = -1;
	ptr = fopen("/home/parsa/.neogit_prop/which_name", "r");
	while (fgets(ssss, sizeof(ssss), ptr) != NULL)
	{
		int nnnn = strlen(ssss);
		if (ssss[nnnn - 1] == '\n') ssss[nnnn - 1] = '\0';
		if (ttt == ttt2) 
		{
			sscanf(ssss, "%d", &stat);
			break;
		}
		ttt2++;
	}
	fclose(ptr);
	if (stat == 1) ptr = fopen("/home/parsa/.neogit_prop/global_name", "r");
	else if (stat == 0)
	{
		char tmpp[1000];
		strcpy(tmpp, repo_add);
		strcat(tmpp, "/.neogit/private_name");
		ptr = fopen(tmpp, "r");
	}
	fgets(ssss, sizeof(ssss), ptr);
	int nnnn = strlen(ssss);
	if (ssss[nnnn - 1] == '\n') ssss[nnnn - 1] = '\0';
	fclose(ptr);
	strcpy(ss, ssss);
	return;
}

void get_author_email(char *ss) // not checked yet !
{
	get_repo_add();
	FILE *ptr = fopen("/home/parsa/.neogit_prop/repo_adds", "r");
	int ttt = 0;
	char ssss[1000];
	while (fgets(ssss, sizeof(ssss), ptr) != NULL)
	{
		int nnnn = strlen(ssss);
		if (ssss[nnnn - 1] == '\n') ssss[nnnn - 1] = '\0';
		if (strcmp(ssss, repo_add) == 0) break;
		ttt++;
	}
	fclose(ptr);
	int ttt2 = 0;
	int stat = -1;
	ptr = fopen("/home/parsa/.neogit_prop/which_email", "r");
	while (fgets(ssss, sizeof(ssss), ptr) != NULL)
	{
		int nnnn = strlen(ssss);
		if (ssss[nnnn - 1] == '\n') ssss[nnnn - 1] = '\0';
		if (ttt == ttt2) 
		{
			sscanf(ssss, "%d", &stat);
			break;
		}
		ttt2++;
	}
	fclose(ptr);
	if (stat == 1) ptr = fopen("/home/parsa/.neogit_prop/global_email", "r");
	else if (stat == 0)
	{
		char tmpp[1000];
		strcpy(tmpp, repo_add);
		strcat(tmpp, "/.neogit/private_email");
		ptr = fopen(tmpp, "r");
	}
	fgets(ssss, sizeof(ssss), ptr);
	int nnnn = strlen(ssss);
	if (ssss[nnnn - 1] == '\n') ssss[nnnn - 1] = '\0';
	fclose(ptr);
	strcpy(ss, ssss);
	return;
}

void get_current_branch(char *ss) // not checked yet !
{
	get_repo_add();
	char tmpp[1000];
	strcpy(tmpp, repo_add);
	strcat(tmpp, "/.neogit/current_branch");
	FILE *ptr = fopen(tmpp, "r");
	char ssss[1000];
	fgets(ssss, sizeof(ssss), ptr);
	int nnnn = strlen(ssss);
	if (ssss[nnnn - 1] == '\n') ssss[nnnn - 1] = '\0';
	fclose(ptr);
	strcpy(ss, ssss);
	return;
}

int get_last_commit_id()
{
	get_repo_add();
	char this_branch[1000];
	get_current_branch(this_branch);
	char tmpp[1000];
	strcpy(tmpp, repo_add);
	strcat(tmpp, "/.neogit/branches/");
	strcat(tmpp, this_branch);
	strcat(tmpp, "/last_commit");
	int cnt = -1;
	FILE *ptr = fopen(tmpp, "r");
	char ssss[1000];
	fgets(ssss, sizeof(ssss), ptr);
	fclose(ptr);
	int nnnn = strlen(ssss);
	if (ssss[nnnn - 1] == '\n') ssss[nnnn - 1] = '\0';
	sscanf(ssss, "%d", &cnt);
	return cnt;
}

void copy_file(char *magh, char *mab)
{
	FILE *p1 = fopen(mab, "r"), *p2 = fopen(magh, "w");
	char ss[1000];
	while (fgets(ss, sizeof(ss), p1) != NULL)
	{
		fprintf(p2, "%s", ss);
	}
	fclose(p1); fclose(p2);
	return;
}

int file_is_empty(char *add) // receives address !
{
	FILE *ptr = fopen(add, "r");
	char ssss[1000];
	bool have_not_enter = false;
	while (fgets(ssss, sizeof(ssss), ptr) != NULL)
	{
		int nnnn = strlen(ssss);
		for (int i = 0; i < nnnn; i++)
		{
			if (ssss[i] != ' ' && ssss[i] != '\t' && ssss[i] != '\n')
			{
				have_not_enter = true;
				break;
			}
		}
	}
	fclose(ptr);
	if (have_not_enter) return 0;
	return 1;
}

int name_equal_to_wild(char *esm, char wild[])
{
	int nesm = strlen(esm), nwild = strlen(wild);
	int fir = -1;
	for (int i = 0; i < nwild; i++)
	{
		if (wild[i] == '*')
		{
			fir = i; break;
		}
	}
	if (fir == -1)
	{
		if (strcmp(esm, wild) == 0) return 1;
		return 0;
	}
	int las = -1;
	for (int i = nwild - 1; i >= 0; i--)
	{
		if (wild[i] == '*')
		{
			las = i; break;
		}
	}
	if (nesm < fir) return 0;
	for (int i = 0; i < fir; i++)
	{
		if (wild[i] != esm[i]) return 0;
	}
	if (nesm < nwild - 1 - las) return 0;
	int tt = nwild - 1 - las;
	for (int i = 1; i <= tt; i++)
	{
		if (esm[nesm - i] != wild[nwild - i]) return 0;
	}
	bool have_harf = false;
	for (int i = fir; i <= las; i++)
	{
		if (wild[i] != '*')
		{
			have_harf = true;
			break;
		}
	}
	if (!have_harf) return 1;
	strcpy(temppp, wild);
	temppp[las] = '\0';
	char *tok = strtok(temppp + fir, "*");
	char *l = esm + fir;
	while (tok != NULL)
	{
		char *cc = strstr(l, tok);
		if (cc == NULL) return 0;
		if (cc + strlen(tok) - 1 - esm >= nesm - tt) return 0;
		l = cc + strlen(tok);
		tok = strtok(NULL, "*");
	}
	return 1;
}

int folder_have_file(char *add, char *esm)
{
	struct dirent *entry;
	DIR *dir = opendir(add);
	while ((entry = readdir(dir)) != NULL)
	{
		if (strcmp(entry->d_name, esm) == 0)
		{
			closedir(dir);
			return 1;
		}
	}
	return 0;
}

int add_num_to_name(char *ss, int num) // return number of extra characters
{
	int tt = 1;
	int sz = strlen(ss);
	ss[sz++] = '#';
	while (num > 0)
	{
		tt++;
		ss[sz++] = '0' + num % 10;
		num /= 10;
	}
	ss[sz] = '\0';
	return tt;
}

int version_of_file(char *ver_haver, char *esm, int MX)
{
	int ans = -1;
	int nnn = strlen(esm);
	for (int i = 1; i <= MX; i++)
	{
		add_num_to_name(esm, i);
		if (strcmp(ver_haver, esm) == 0) ans = i;
		esm[nnn] = '\0';
		if (ans != -1) break;
	}
	return ans;
}


void run_init(int argc, char *argv[])
{
	// size of address : 1000
	// return 1 ?!
	if (argc != 2)
	{
		printf("please enter a valid command!\n");
		return;
	}
	char now_address[1000];
	getcwd(now_address, sizeof(now_address));
	struct dirent *entry;
	bool vojood = false;
	char tmp_add[1000];
	while (true)
	{
		DIR *dir = opendir(".");
		while ((entry = readdir(dir)) != NULL)
		{
			if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) continue;
			if (entry->d_type == DT_DIR && strcmp(entry->d_name, ".neogit") == 0)
			{
				vojood = true; break;
			}
		}
		closedir(dir);
		getcwd(tmp_add, sizeof(tmp_add));
		if (vojood) break;
		if (strcmp(tmp_add, "/") == 0) break;
		chdir("..");
	}
	chdir(now_address);
	if (vojood)
	{
		printf("repo already exists!\n");
		return;
	}
	/// check whether global name and email exist ///
	FILE *ptr = fopen("/home/parsa/.neogit_prop/global_name", "r");
	bool have_it = false;
	char ssss[200];
	while (fgets(ssss, sizeof(ssss), ptr) != NULL)
	{
		int nnnn = strlen(ssss);
		for (int i = 0; i < nnnn; i++)
		{
			if (ssss[i] != ' ' && ssss[i] != '\t' && ssss[i] != '\n')
			{
				have_it = true;
				break;
			}
		}
	}
	fclose(ptr);
	if (!have_it)
	{
		printf("you have not set a global name!\n");
		return;
	}
	ptr = fopen("/home/parsa/.neogit_prop/global_email", "r");
	have_it = false;
	while (fgets(ssss, sizeof(ssss), ptr) != NULL)
	{
		int nnnn = strlen(ssss);
		for (int i = 0; i < nnnn; i++)
		{
			if (ssss[i] != ' ' && ssss[i] != '\t' && ssss[i] != '\n')
			{
				have_it = true;
				break;
			}
		}
	}
	fclose(ptr);
	if (!have_it)
	{
		printf("you have not set a global email!\n");
		return;
	}
	char tmppp[1000];
	getcwd(tmppp, sizeof(tmppp));
	ptr = fopen("/home/parsa/.neogit_prop/which_name", "a");
	fprintf(ptr, "1\n");
	fclose(ptr);
	ptr = fopen("/home/parsa/.neogit_prop/which_email", "a");
	fprintf(ptr, "1\n");
	fclose(ptr);
	ptr = fopen("/home/parsa/.neogit_prop/repo_adds", "a");
	fprintf(ptr, "%s\n", tmppp);
	fclose(ptr);
	mkdir(".neogit", 0755);
	ptr = fopen(".neogit/private_name", "w");
	fclose(ptr);
	ptr = fopen(".neogit/private_email", "w");
	fclose(ptr);
	printf("repo created successfully!\n");
	ptr = fopen(".neogit/staging", "w");
	fclose(ptr);
	ptr = fopen(".neogit/number_of_commits", "w");
	fprintf(ptr, "0");
	fclose(ptr);
	ptr = fopen(".neogit/current_branch", "w");
	fprintf(ptr, "master");
	fclose(ptr);
	ptr = fopen(".neogit/branch_names", "w");
	fprintf(ptr, "master\n");
	fclose(ptr);
	mkdir(".neogit/commits", 0755);
	mkdir(".neogit/commits/0", 0755);
	ptr = fopen(".neogit/commits/0/info", "w"); // not checked yet !
	fclose(ptr);
	ptr = fopen(".neogit/commits/0/files", "w"); // not checked yet !
	fclose(ptr);
	mkdir(".neogit/branches", 0755);
	mkdir(".neogit/branches/master", 0755);
	ptr = fopen(".neogit/branches/master/last_commit", "w");
	fprintf(ptr, "0");
	fclose(ptr);
	ptr = fopen(".neogit/branches/master/commits", "w"); // not checked yet !
	fclose(ptr);
	ptr = fopen(".neogit/last_seen_commit", "w"); // not checked yet !
	fprintf(ptr, "0");
	fclose(ptr);
	ptr = fopen(".neogit/add_history_cnt", "w"); // not checked yet !
	fprintf(ptr, "0");
	fclose(ptr);
	mkdir(".neogit/add_history", 0755); // not checked yet !
	mkdir(".neogit/alias", 0755);
	ptr = fopen(".neogit/alias/first", "w");
	fclose(ptr);
	ptr = fopen(".neogit/alias/second", "w");
	fclose(ptr);
	mkdir(".neogit/shortcuts", 0755); // check ?
	ptr = fopen(".neogit/shortcuts/first", "w");
	fclose(ptr);
	ptr = fopen(".neogit/shortcuts/second", "w");
	fclose(ptr);
	ptr = fopen(".neogit/reset_history", "w");
	fclose(ptr);
	ptr = fopen(".neogit/tag_names", "w");
	fclose(ptr);
	mkdir(".neogit/tags", 0755);
	ptr = fopen(".neogit/applied_hooks", "w");
	fclose(ptr);
	ptr = fopen(".neogit/hooks_list", "w");
	fprintf(ptr, "todo-check\neof-blank-space\ncharacter-limit\n");
	fclose(ptr);
	return;
}

int get_number_of_versions(char *esm) // receives esm
{
	char now_add[1000];
	getcwd(now_add, sizeof(now_add));
	get_repo_add();
	chdir(repo_add);
	chdir(".neogit");
	struct dirent *entry;
	DIR *dir = opendir(".");
	int cnt = -1;
	while ((entry = readdir(dir)) != NULL)
	{
		if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0 || strcmp(entry->d_name, ".neogit") == 0) continue;
		if (strcmp(entry->d_name, esm) == 0 && entry->d_type == DT_REG)
		{
			FILE *ptr = fopen(esm, "r");
			char sss[100];
			fgets(sss, 100, ptr);
			sscanf(sss, "%d", &cnt);
			fclose(ptr);
			break;
		}
	}
	chdir(now_add);
	closedir(dir);
	if (cnt == -1) cnt = 0;
	return cnt;
}

int all_changes_are_committed() // not checked - CHECK IT!!
{
	get_repo_add();
	int last_commit_num = get_last_commit_id();
	get_all_repo_files();
	char tmpp[1000];
	strcpy(tmpp, repo_add);
	strcat(tmpp, "/.neogit/commits/");
	int xxx = last_commit_num;
	int nnnn = strlen(tmpp);
	if (xxx == 0) tmpp[nnnn++] = '0';
	while (xxx > 0)
	{
		tmpp[nnnn++] = xxx % 10 + '0';
		xxx /= 10;
	}
	tmpp[nnnn] = '\0';
	strcat(tmpp, "/files");
	copy_file_into_array(tmpp);
	if (size_of_copied_file_array != number_of_all_repo_files) return 0;
	char ssss[1000], tmpp2[1000];
	bool it_is_ok = true;
	for (int i = 0; i < number_of_all_repo_files; i++)
	{
		strcpy(ssss, all_repo_files[i]);
		change_add_to_name(ssss);
		int cnt_have = get_number_of_versions(ssss);
		for (int j = 0; j < size_of_copied_file_array; j++)
		{
			if (version_of_file(copied_file_array[j], ssss, cnt_have + 3) != -1)
			{
				strcpy(tmpp2, repo_add);
				strcat(tmpp2, "/.neogit/");
				strcat(tmpp2, copied_file_array[j]);
				if (!files_are_equal(all_repo_files[i], tmpp2)) 
				{
					it_is_ok = false;
					break;
				}
			}
		}
	}
	if (it_is_ok) return 1;
	return 0;
}

int is_in_staging_area(char *addd) // receives address!
{
	get_repo_add();
	char now_add[1000];
	getcwd(now_add, sizeof(now_add));
	chdir(repo_add);
	chdir(".neogit");
	FILE *ptr = fopen("staging", "r");
	char sss[1000];
	change_add_to_name(addd);
	bool we_have_it = false;
	while (fgets(sss, sizeof(sss), ptr) != NULL)
	{
		int nnn2 = strlen(sss);
		if (sss[nnn2 - 1] == '\n') sss[nnn2 - 1] = '\0';
		int cnt = get_number_of_versions(addd);
		if (version_of_file(sss, addd, cnt + 3) != -1) 
		{
			we_have_it = true;
			break;
		}
	}
	fclose(ptr);
	chdir(now_add);
	change_name_to_add(addd);
	if (we_have_it) return 1;
	return 0;
}

void add_one_file(char *esm)
{
	get_repo_add();
	char now_add[1000], first_add[1000], second_add[1000];
	strcpy(first_add, esm);
	if (Flag)
	{
		fprintf(PTR_global, "%s\n", esm);
	}
	change_add_to_name(esm);
	getcwd(now_add, sizeof(now_add));
	chdir(repo_add);
	chdir(".neogit");
	int cnt = get_number_of_versions(esm); /// check later !
	char ffff[1000];
	getcwd(ffff, sizeof(ffff));
	FILE *ptr = fopen(esm, "w");
	fprintf(ptr, "%d", cnt + 1);
	fclose(ptr);
	strcpy(second_add, repo_add);
	strcat(second_add, "/.neogit");
	cnt++;
	int nnn = strlen(esm);
	add_num_to_name(esm, cnt);
	strcat(second_add, "/");
	strcat(second_add, esm);
	esm[nnn] = '\0';
	copy_file(second_add, first_add);
	FILE *sta = fopen("staging", "r"), *tmp_sta = fopen("tmp_staging", "w");
	char sss[1000];
	while (fgets(sss, sizeof(sss), sta) != NULL)
	{
		int tttt = strlen(sss);
		bool ffat = false;
		if (sss[tttt - 1] == '\n') 
		{
			sss[tttt - 1] = '\0';
			ffat = true;
		}
		if (version_of_file(sss, esm, cnt + 3) == -1) 
		{
			fprintf(tmp_sta, "%s", sss);
			if (ffat) fprintf(tmp_sta, "\n");
		}
	}
	fclose(sta); fclose(tmp_sta);
	remove("staging");
	rename("tmp_staging", "staging");
	ptr = fopen("staging", "a");
	nnn = strlen(esm);
	add_num_to_name(esm, cnt);
	fprintf(ptr, "%s\n", esm);
	fclose(ptr);
	esm[nnn] = '\0';
	chdir(now_add);
	return;
}

void add_one_folder(char *folder_add)
{
	DIR *dir = opendir(folder_add);
	struct dirent *entry;
	char tmpppp[1000];
	while ((entry = readdir(dir)) != NULL)
	{
		strcpy(tmpppp, folder_add);
		if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0 || strcmp(entry->d_name, ".neogit") == 0) continue;
		if (entry->d_type == DT_REG)
		{
			strcat(tmpppp, "/");
			strcat(tmpppp, entry->d_name);
			add_one_file(tmpppp);
			continue;
		}
		if (entry->d_type == DT_DIR)
		{
			strcat(tmpppp, "/");
			strcat(tmpppp, entry->d_name);
			add_one_folder(tmpppp);
		}
	}
	closedir(dir);
	return;
}

void add_depth_folder(char *add, int dep)
{
	if (dep <= 0) return;
	DIR *dir = opendir(add);
	struct dirent *entry;
	char tmpp[1000];
	while ((entry = readdir(dir)) != NULL)
	{
		if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0 || strcmp(entry->d_name, ".neogit") == 0) continue;
		strcpy(tmpp, add);
		strcat(tmpp, "/");
		strcat(tmpp, entry->d_name);
		if (entry->d_type == DT_REG)
		{
			char ssss[30];
			if (is_in_staging_area(tmpp)) strcpy(ssss, "staged!");
			else strcpy(ssss, "not staged!");
			printf("%s (depth %d) %s\n", entry->d_name, DDDD - dep, ssss);
		}
		if (entry->d_type == DT_DIR)
		{
			add_depth_folder(tmpp, dep - 1);
		}
	}
	return;
}

void run_add_depth(int argc, char *argv[])
{
	if (argc != 4)
	{
		printf("please enter a valid command!\n");
		return;
	}
	int nnn = strlen(argv[3]);
	for (int i = 0; i < nnn; i++)
	{
		if (!(argv[3][i] >= '0' && argv[3][i] <= '9'))
		{
			printf("please enter a valid command!\n");
			return;
		}
	}
	int dep = -1;
	sscanf(argv[3], "%d", &dep);
	if (dep < 1)
	{
		printf("please enter a valid command!\n");
		return;
	}
	char now_add[1000];
	getcwd(now_add, sizeof(now_add));
	DDDD = dep + 1;
	add_depth_folder(now_add, dep);
	chdir(now_add);
	return;
}

void run_add(int argc, char *argv[])
{
	if (argc <= 2)
	{
		printf("please enter a valid command!\n");
		return;
	}
	if (strcmp(argv[2], "-redo") == 0)
	{
		if (argc != 3)
		{
			printf("please enter a valid command!\n");
			return;
		}
		get_repo_add();
		char tmpp[1000];
		strcpy(tmpp, repo_add);
		strcat(tmpp, "/.neogit/reset_history");
		FILE *ptr = fopen(tmpp, "r");
		char ssss[1000];
		while (fgets(ssss, sizeof(ssss), ptr) != NULL)
		{
			remove_the_enter(ssss);
			add_one_file(ssss);
		}
		fclose(ptr);
		empty_reset_history();
		return;
	}
	int ind = 2;
	if (strcmp(argv[2], "-n") == 0)
	{
		run_add_depth(argc, argv);
		return;
	}
	if (strcmp(argv[2], "-f") == 0) ind++;
	if (ind >= argc)
	{
		printf("please enter a valid command!\n");
		return;
	}
	struct dirent *entry;
	char now_add[1000], tmpp[1000], tmp_very_fat[1000];
	int add_number = get_add_history_cnt() + 1;
	strcpy(tmp_very_fat, repo_add);
	strcat(tmp_very_fat, "/.neogit/add_history_cnt");
	FILE *ptr_very_fat = fopen(tmp_very_fat, "w");
	fprintf(ptr_very_fat, "%d", add_number);
	fclose(ptr_very_fat);
	strcpy(tmp_very_fat, repo_add);
	strcat(tmp_very_fat, "/.neogit/add_history/");
	int xxx = add_number;
	int nnnn = strlen(tmp_very_fat);
	if (xxx == 0) tmp_very_fat[nnnn++] = '0';
	while (xxx > 0)
	{
		tmp_very_fat[nnnn++] = xxx % 10 + '0';
		xxx /= 10;
	}
	tmp_very_fat[nnnn] = '\0';
	PTR_global = fopen(tmp_very_fat, "w");
	Flag = true;
	while (ind < argc)
	{
		DIR *dir = opendir(".");
		bool have_the_file = false;
		while ((entry = readdir(dir)) != NULL)
		{
			if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0 || strcmp(entry->d_name, ".neogit") == 0) continue;
			if (name_equal_to_wild(entry->d_name, argv[ind])) // i or ind ?
			{
				have_the_file = true;
				getcwd(tmpp, sizeof(tmpp));
				if (entry->d_type == DT_REG)
				{
					getcwd(now_add, sizeof(now_add));
					strcat(tmpp, "/");
					strcat(tmpp, entry->d_name);
					add_one_file(tmpp);
					chdir(now_add);
					continue;
				}
				
				if (entry->d_type == DT_DIR)
				{
					getcwd(now_add, sizeof(now_add));
					strcat(tmpp, "/");
					strcat(tmpp, entry->d_name);
					add_one_folder(tmpp);
					chdir(now_add);
					continue;
				}
			}
		}
		if (!have_the_file)
			printf("no file mathcing with %s found!\n", argv[ind]);

		closedir(dir);
		ind++;
	}
	fclose(PTR_global);
	Flag = false;
	return;
}

void run_config_global(int argc, char *argv[])
{
	if (argc <= 3)
	{
		printf("please enter a valid command!\n");
		return;
	}
	if (strcmp(argv[3], "user.name") != 0 && strcmp(argv[3], "user.email") != 0)
	{
		printf("please enter a valid command!\n");
		return;
	}
	if (argc != 5)
	{
		printf("please enter a valid command!\n");
		return;
	}
	if (strcmp(argv[3], "user.name") == 0)
	{
		FILE *ptr = fopen("/home/parsa/.neogit_prop/global_name", "w");
		fprintf(ptr, "%s", argv[4]);
		fclose(ptr);
		ptr = fopen("/home/parsa/.neogit_prop/which_name", "r");
		int ttt = 0;
		char sss[50];
		while (fgets(sss, sizeof(sss), ptr) != NULL)
		{
			if (sss[0] != ' ' && sss[0] != '\t' && sss[0] != '\n') ttt++;
		}
		fclose(ptr);
		ptr = fopen("/home/parsa/.neogit_prop/which_name", "w");
		for (int i = 0; i < ttt; i++) fprintf(ptr, "1\n");
		return;
	}
	FILE *ptr = fopen("/home/parsa/.neogit_prop/global_email", "w");
	fprintf(ptr, "%s", argv[4]);
	fclose(ptr);
	ptr = fopen("/home/parsa/.neogit_prop/which_email", "r");
	int ttt = 0;
	char sss[50];
	while (fgets(sss, sizeof(sss), ptr) != NULL)
	{
		if (sss[0] != ' ' && sss[0] != '\t' && sss[0] != '\n') ttt++;
	}
	fclose(ptr);
	ptr = fopen("/home/parsa/.neogit_prop/which_email", "w");
	for (int i = 0; i < ttt; i++) fprintf(ptr, "1\n");
	return;
}

void run_alias(int argc, char *argv[])
{
	char command_name[1000];
	strcpy(command_name, argv[2] + 6);
	get_repo_add();
	char tmpp[1000];
	strcpy(tmpp, repo_add);
	strcat(tmpp, "/.neogit/alias/first");
	FILE *ptr = fopen(tmpp, "a");
	fprintf(ptr, "%s\n", command_name);
	fclose(ptr);
	strcpy(tmpp, repo_add);
	strcat(tmpp, "/.neogit/alias/second");
	ptr = fopen(tmpp, "a");
	fprintf(ptr, "%s\n", argv[3]);
	fclose(ptr);
	return;
}

void run_alias_global(int argc, char *argv[])
{
	char command_name[1000];
	strcpy(command_name, argv[3] + 6);
	char tmpp[1000];
	strcpy(tmpp, "/home/parsa/.neogit_prop/alias/first");
	FILE *ptr = fopen(tmpp, "a");
	fprintf(ptr, "%s\n", command_name);
	fclose(ptr);
	strcpy(tmpp, "/home/parsa/.neogit_prop/alias/second");
	ptr = fopen(tmpp, "a");
	fprintf(ptr, "%s\n", argv[4]);
	fclose(ptr);
	return;
}

void run_config(int argc, char *argv[])  // alias not added !!! //
{
	if (argc <= 2)
	{
		printf("please enter a valid command!\n");
		return;
	}
	if (strstr(argv[2], "alias.") == argv[2])
	{
		if (strcmp(argv[2], "alias.") == 0)
		{
			printf("please enter alias name!\n");
			return;
		}
		if (argc != 4)
		{
			printf("please enter a valid command!\n");
			return;
		}
		get_repo_add();
		if (strlen(repo_add) == 0)
		{
			printf("please create repo first!\n");
			return;
		}
		run_alias(argc, argv);
		return;
	}
	if (strcmp(argv[2], "-global") == 0)
	{
		if (argc <= 3)
		{
			printf("please enter a valid command!\n");
			return;
		}
		if (strstr(argv[3], "alias.") == argv[3])
		{
			if (strcmp(argv[3], "alias.") == 0)
			{
				printf("please enter a valid command!\n");
				return;
			}
			if (argc != 5)
			{
				printf("please enter a valid command!\n");
				return;
			}
			run_alias_global(argc, argv);
			return;
		}
	}
	if (strcmp(argv[2], "-global") == 0)
	{
		// suppose alias is handled !!! //
		run_config_global(argc, argv);
		return;
	}
	// check if repo exists !!! //
	// suppose alias is handled before !!! //
	get_repo_add();
	if (strlen(repo_add) == 0)
	{
		printf("please create repo first!\n");
		return;
	}
	if (strcmp(argv[2], "user.name") != 0 && strcmp(argv[2], "user.email") != 0)
	{
		printf("please enter a valid command!\n");
		return;
	}
	if (argc != 4)
	{
		printf("please enter a valid command!\n");
		return;
	}
	if (strcmp(argv[2], "user.name") == 0)
	{
		char tmpppp[1000];
		strcpy(tmpppp, repo_add);
		strcat(tmpppp, "/.neogit/private_name");
		FILE *ptr = fopen(tmpppp, "w");
		fprintf(ptr, "%s", argv[3]);
		fclose(ptr);
		char now_add[1000];
		getcwd(now_add, sizeof(now_add));
		chdir("/home/parsa/.neogit_prop");
		ptr = fopen("repo_adds", "r");
		int t_line = 0;
		char ssss[1000];
		while (fgets(ssss, sizeof(ssss), ptr) != NULL)
		{
			int nnnn = strlen(ssss);
			if (ssss[nnnn - 1] == '\n') ssss[nnnn - 1] = '\0';
			if (strcmp(ssss, repo_add) == 0) break;
			t_line++;
		}
		fclose(ptr);
		FILE *ptr1 = fopen("which_name", "r"), *ptr2 = fopen("tmp_file", "w");
		int ttt2 = 0;
		while (fgets(ssss, sizeof(ssss), ptr1) != NULL)
		{
			int nnnn = strlen(ssss);
			bool have_enter = false;
			if (ssss[nnnn - 1] == '\n')
			{
				ssss[nnnn - 1] = '\0';
				have_enter = true;
			}
			if (ttt2 == t_line)
			{
				ssss[0] = '0';
			}
			if (have_enter) ssss[nnnn - 1] = '\n';
			ttt2++;
			fprintf(ptr2, "%s", ssss);
		}
		fclose(ptr1); fclose(ptr2);
		remove("which_name");
		rename("tmp_file", "which_name");
		chdir(now_add);
		return;
	}
	char tmpppp[1000];
	strcpy(tmpppp, repo_add);
	strcat(tmpppp, "/.neogit/private_email");
	FILE *ptr = fopen(tmpppp, "w");
	fprintf(ptr, "%s", argv[3]);
	fclose(ptr);
	char now_add[1000];
	getcwd(now_add, sizeof(now_add));
	chdir("/home/parsa/.neogit_prop");
	ptr = fopen("repo_adds", "r");
	int t_line = 0;
	char ssss[1000];
	while (fgets(ssss, sizeof(ssss), ptr) != NULL)
	{
		int nnnn = strlen(ssss);
		if (ssss[nnnn - 1] == '\n') ssss[nnnn - 1] = '\0';
		if (strcmp(ssss, repo_add) == 0) break;
		t_line++;
	}
	fclose(ptr);
	FILE *ptr1 = fopen("which_email", "r"), *ptr2 = fopen("tmp_file", "w");
	int ttt2 = 0;
	while (fgets(ssss, sizeof(ssss), ptr1) != NULL)
	{
		int nnnn = strlen(ssss);
		bool have_enter = false;
		if (ssss[nnnn - 1] == '\n')
		{
			ssss[nnnn - 1] = '\0';
			have_enter = true;
		}
		if (ttt2 == t_line)
		{
			ssss[0] = '0';
		}
		if (have_enter) ssss[nnnn - 1] = '\n';
		ttt2++;
		fprintf(ptr2, "%s", ssss);
	}
	fclose(ptr1); fclose(ptr2);
	remove("which_email");
	rename("tmp_file", "which_email");
	chdir(now_add);
	return;
}

int get_number_of_all_commits()
{
	get_repo_add();
	char tmpp[1000];
	strcpy(tmpp, repo_add);
	strcat(tmpp, "/.neogit/number_of_commits");
	FILE *ptr = fopen(tmpp, "r");
	char ssss[1000];
	fgets(ssss, sizeof(ssss), ptr);
	int cnt = -1;
	int nnnn = strlen(ssss);
	if (ssss[nnnn - 1] == '\n') ssss[nnnn - 1] = '\0';
	sscanf(ssss, "%d", &cnt);
	fclose(ptr);
	return cnt;
}

void run_commit(int argc, char *argv[]) // shortcut not handled ! //
{
	if (argc <= 2)
	{
		printf("please enter a valid command!\n");
		return;
	}
	if (strcmp(argv[2], "-m") != 0 && strcmp(argv[2], "-s") != 0)
	{
		printf("please enter a valid command!\n");
		return;
	}
	if (strcmp(argv[2], "-s") == 0)
	{
		if (argc != 4)
		{
			printf("please enter a valid command!\n");
			return;
		}
		get_repo_add();
		char tmpp[1000], ssss[1000], tmpp2[1000];
		strcpy(tmpp, repo_add);
		strcat(tmpp, "/.neogit/shortcuts/first");
		strcpy(tmpp2, repo_add);
		strcat(tmpp2, "/.neogit/shortcuts/second");
		FILE *ptr1 = fopen(tmpp, "r");
		bool have_it = false;
		int ttt = 0;
		while (fgets(ssss, sizeof(ssss), ptr1) != NULL)
		{
			remove_the_enter(ssss);
			if (strcmp(ssss, argv[3]) == 0)
			{
				have_it = true;
				break;
			}
			ttt++;
		}
		if (!have_it)
		{
			printf("shortcut-name does not exist!\n");
			return;
		}
		int ttt2 = 0;
		ptr1 = fopen(tmpp2, "r");
		while (fgets(ssss, sizeof(ssss), ptr1) != NULL)
		{
			remove_the_enter(ssss);
			if (ttt == ttt2) break;
			ttt2++;
		}
		strcpy(argv[2], "-m");
		strcpy(argv[3], ssss);
	}
	if (strcmp(argv[2], "-m") == 0)
	{
		if (argc <= 3)
		{
			printf("you have not entered the commit message!\n");
			return;
		}
		if (argc != 4)
		{
			printf("invalid message!\n");   /// think later :)))
			return;
		}
		if (strlen(argv[3]) > 72)
		{
			printf("character limit exceeded!\n");
			return;
		}
		char tmpp[1000];
		strcpy(tmpp, repo_add);
		strcat(tmpp, "/.neogit/staging");
		if (file_is_empty(tmpp))
		{
			printf("staging area is empty!\n");
			return;
		}
		int commit_id = get_number_of_all_commits() + 1; // number_of_commits not updated yet ! //
		char what_time[1000], what_name[1000], what_email[1000], what_branch[1000];
		get_the_time(what_time);
		get_author_name(what_name);
		get_author_email(what_email);
		get_current_branch(what_branch);
		int par_commit = get_last_commit_id();
		/*
		   add staging		DONE
		   empty staging		DONE
		   add parent commit files		DONE
		   number_of_commits++		DONE
		   make new folder in commits		DONE
		   change last_commit in branches/<branch_name>		DONE
		   add to branches/<branch_name>/commits		DONE
		*/
		get_repo_add();
		strcpy(tmpp, repo_add);
		strcat(tmpp, "/.neogit/number_of_commits");
		FILE *ptr = fopen(tmpp, "w");
		fprintf(ptr, "%d", commit_id);
		fclose(ptr);
		strcpy(tmpp, repo_add);
		strcat(tmpp, "/.neogit/commits/");
		int xx = commit_id;
		int nnnn = strlen(tmpp);
		if (xx == 0) tmpp[nnnn++] = '0';
		while (xx > 0)
		{
			tmpp[nnnn++] = xx % 10 + '0';
			xx /= 10;
		}
		tmpp[nnnn] = '\0';
		mkdir(tmpp, 0755);
		char tmpp2[1000], tmp_repo[1000], tmpp3[1000];
		strcpy(tmpp2, tmpp);
		strcpy(tmpp3, tmpp);
		strcat(tmpp2, "/info");
		strcat(tmpp3, "/files");
		ptr = fopen(tmpp2, "w");
		fprintf(ptr, "%d\n", commit_id);
		fprintf(ptr, "%s\n", what_branch);
		fprintf(ptr, "%d\n", par_commit);
		fprintf(ptr, "%s\n", what_time);
		fprintf(ptr, "%s\n", what_name);
		fprintf(ptr, "%s\n", what_email);
		fprintf(ptr, "%s\n", argv[3]);
		strcpy(tmp_repo, repo_add);
		strcat(tmp_repo, "/.neogit/staging");
		FILE *ptr_staging = fopen(tmp_repo, "r");
		char ssstaging[1000];
		int cnt_line = 0;
		while (fgets(ssstaging, sizeof(ssstaging), ptr_staging) != NULL)
		{
			remove_the_enter(ssstaging);
			int nnnn = strlen(ssstaging);
			bool is_not_empty = false;
			for (int i = 0; i < nnnn; i++)
			{
				if (ssstaging[i] != ' ' && ssstaging[i] != '\t' && ssstaging[i] != '\n')
				{
					is_not_empty = true;
					break;
				}
			}
			if (is_not_empty) cnt_line++;
		}
		fclose(ptr_staging);
		fprintf(ptr, "%d\n", cnt_line);
		fclose(ptr);
		copy_file(tmpp3, tmp_repo);
		ptr_staging = fopen(tmp_repo, "w");
		fclose(ptr_staging);
		char parent_add[1000];
		strcpy(parent_add, repo_add);
		strcat(parent_add, "/.neogit/commits/");
		int xxx = par_commit;
		int nn_par = strlen(parent_add);
		if (xxx == 0) parent_add[nn_par++] = '0';
		while (xxx > 0)
		{
			parent_add[nn_par++] = xxx % 10 + '0';
			xxx /= 10;
		}
		parent_add[nn_par] = '\0';
		strcat(parent_add, "/files");
		FILE *par_files = fopen(parent_add, "r");
		char ss_par[1000], ss_cur[1000];
		while (fgets(ss_par, sizeof(ss_par), par_files) != NULL) // this while is not checked !
		{
			remove_the_enter(ss_par);
			FILE *cur_commit_ptr = fopen(tmpp3, "r");
			bool file_in_staging = false;
			while (fgets(ss_cur, sizeof(ss_cur), cur_commit_ptr) != NULL)
			{
				remove_the_enter(ss_cur);
				remove_num_from_name(ss_cur);
				if (version_of_file(ss_par, ss_cur, get_number_of_versions(ss_cur) + 3) != -1)
				{
					file_in_staging = true;
					break;
				}
			}
			fclose(cur_commit_ptr);
			// check if ss_par is correctly restored !!!! //
			bool really_have = false;
			int indXX = remove_num_from_name(ss_par);
			change_name_to_add(ss_par);
			if (file_really_exists(ss_par)) really_have = true;
			change_add_to_name(ss_par);
			ss_par[indXX] = '#';
			if ((!file_in_staging) && really_have)
			{
				cur_commit_ptr = fopen(tmpp3, "a");
				fprintf(cur_commit_ptr, "%s\n", ss_par);
				fclose(cur_commit_ptr);
			}
		}
		fclose(par_files);
		char bra_add[1000];
		strcpy(bra_add, repo_add);
		strcat(bra_add, "/.neogit/branches/");
		strcat(bra_add, what_branch);
		char bad_file[1000];
		strcpy(bad_file, bra_add);
		strcat(bad_file, "/last_commit");
		FILE *file_ptr = fopen(bad_file, "w");
		fprintf(file_ptr, "%d", commit_id);
		fclose(file_ptr);
		strcpy(bad_file, bra_add);
		strcat(bad_file, "/commits");
		file_ptr = fopen(bad_file, "a");
		fprintf(file_ptr, "%d\n", commit_id);
		fclose(file_ptr);
		printf("commited successfully!\n");
		printf("commit id: %d\n", commit_id);
		printf("time: %s\n", what_time);
		printf("commit message: %s\n", argv[3]);
		get_repo_add();
		char very_fat_add[1000];
		strcpy(very_fat_add, repo_add);
		strcat(very_fat_add, "/.neogit/last_seen_commit");
		FILE *ptr_fat = fopen(very_fat_add, "w");
		fprintf(ptr_fat, "%d", commit_id);
		fclose(ptr_fat);
		empty_add_history();
		empty_reset_history();
	}
	return;
}

void print_all_branches()
{
	get_repo_add();
	char tmpp[1000];
	strcpy(tmpp, repo_add);
	strcat(tmpp, "/.neogit/branch_names");
	FILE *ptr = fopen(tmpp, "r");
	char ssss[1000];
	while (fgets(ssss, sizeof(ssss), ptr) != NULL)
	{
		remove_the_enter(ssss);
		printf("%s\n", ssss);
	}
	fclose(ptr);
	return;
}

void run_branch(int argc, char *argv[])
{
	if (argc == 2)
	{
		print_all_branches();
		return;
	}
	if (argc != 3)
	{
		printf("please enter a valid command!\n");
		return;
	}
	/*
	   add name to branch_names 	DONE
	   add a folder to branches 	DONE
	   		commits file	DONE
			last_commit file -> from get_last_commit_id()	DONE
	*/
	get_repo_add();
	char tmpp[1000];
	strcpy(tmpp, repo_add);
	strcat(tmpp, "/.neogit/branch_names");
	char ssss[1000];
	FILE *ptr = fopen(tmpp, "r");
	bool have_it = false;
	while (fgets(ssss, sizeof(ssss), ptr) != NULL)
	{
		remove_the_enter(ssss);
		if (strcmp(ssss, argv[2]) == 0)
		{
			have_it = true;
			break;
		}
	}
	fclose(ptr);
	if (have_it)
	{
		printf("branch name already exists!\n");
		return;
	}
	ptr = fopen(tmpp, "a");
	fprintf(ptr, "%s\n", argv[2]);
	fclose(ptr);
	strcpy(tmpp, repo_add);
	strcat(tmpp, "/.neogit/branches/");
	strcat(tmpp, argv[2]);
	mkdir(tmpp, 0755);
	char tmpp2[1000];
	strcpy(tmpp2, tmpp);
	strcat(tmpp2, "/commits");
	ptr = fopen(tmpp2, "w");
	fclose(ptr);
	int ffatt = get_last_commit_id();
	strcpy(tmpp2, tmpp);
	strcat(tmpp2, "/last_commit");
	ptr = fopen(tmpp2, "w");
	fprintf(ptr, "%d", ffatt);
	fclose(ptr);
	return;
}

void checkout_to_commit(int com_id) // with checking
{
	get_repo_add();
	char tmpp[1000];
	strcpy(tmpp, repo_add);
	strcat(tmpp, "/.neogit/staging");
	if (!file_is_empty(tmpp))
	{
		printf("staging area is not empty!\n");
		return;
	}
	char add_fat[1000];
	strcpy(add_fat, repo_add);
	strcat(add_fat, "/.neogit/last_seen_commit");
	FILE *ptr_fattar = fopen(add_fat, "r");
	char ssstrrr[100];
	fgets(ssstrrr, sizeof(ssstrrr), ptr_fattar);
	fclose(ptr_fattar);
	remove_the_enter(ssstrrr);
	int last_seen_com = -1;
	sscanf(ssstrrr, "%d", &last_seen_com);
	if ((!all_changes_are_committed()) && last_seen_com == get_last_commit_id())
	{
		printf("some changes have not been committed!\n");
		return;
	}
	get_all_repo_files();
	for (int i = 0; i < number_of_all_repo_files; i++) remove(all_repo_files[i]);
	strcpy(tmpp, repo_add);
	strcat(tmpp, "/.neogit/commits/");
	int xxx = com_id;
	int nnnn = strlen(tmpp);
	if (xxx == 0) tmpp[nnnn++] = '0';
	while (xxx > 0)
	{
		tmpp[nnnn++] = xxx % 10 + '0';
		xxx /= 10;
	}
	tmpp[nnnn++] = '\0';
	strcat(tmpp, "/files");
	FILE *ptr = fopen(tmpp, "r");
	char ssss[1000], mab_file[1000];
	while (fgets(ssss, sizeof(ssss), ptr) != NULL)
	{
		remove_the_enter(ssss);
		strcpy(mab_file, repo_add);
		strcat(mab_file, "/.neogit/");
		strcat(mab_file, ssss);
		int indx = -1;
		int nnn2 = strlen(ssss);
		for (int i = nnn2 - 1; i >= 0; i--)
		{
			if (ssss[i] == '#')
			{
				indx = i; break;
			}
		}
		ssss[indx] = '\0';
		change_name_to_add(ssss);
		copy_file(ssss, mab_file);
	}
	fclose(ptr);
	ptr_fattar = fopen(add_fat, "w");
	fprintf(ptr_fattar, "%d", com_id);
	fclose(ptr_fattar);
	empty_add_history();
	empty_reset_history();
	printf("checkout was successful!\n");
	return;
}

void run_checkout(int argc, char *argv[]) // without HEAD-n
{
	if (argc != 3)
	{
		printf("please enter a valid command!\n");
		return;
	}
	if (strcmp(argv[2], "HEAD") == 0)
	{
		int ppp = get_last_commit_id();
		checkout_to_commit(ppp);
		return;
	}
	if (strstr(argv[2], "HEAD-") == argv[2])
	{
		if (strcmp(argv[2], "HEAD-") == 0)
		{
			printf("please enter a valid command!\n");
			return;
		}
		int num = -1, ccnt = -1;
		char *ccc = argv[2] + 5;
		sscanf(ccc, "%d%n", &num, &ccnt);
		if (ccnt != strlen(ccc) || num <= 0)
		{
			printf("please enter a valid number!\n");
			return;
		}
		int ppp = get_last_commit_id();
		for (int i = 1; i <= num; i++) 
		{
			ppp = get_parent_of_commit(ppp);
			if (ppp == 0)
			{
				ppp = -1;
				break;
			}
		}
		if (ppp == -1)
		{
			printf("please enter a valid number!\n");
			return;
		}
		checkout_to_commit(ppp);
		return;
	}
	get_repo_add();
	bool have_not_number = false;
	int nnnn = strlen(argv[2]);
	for (int i = 0; i < nnnn; i++)
	{
		if (!(argv[2][i] >= '0' && argv[2][i] <= '9'))
		{
			have_not_number = true;
			break;
		}
	}
	if (!have_not_number)
	{
		int xx = -1;
		sscanf(argv[2], "%d", &xx);
		if (xx <= 0 || xx > get_number_of_all_commits())
		{
			printf("invalid commit id!\n");
			return;
		}
		checkout_to_commit(xx);
		return;
	}
	char tmpp[1000];
	strcpy(tmpp, repo_add);
	strcat(tmpp, "/.neogit/branch_names");
	FILE *ptr = fopen(tmpp, "r");
	char ssss[1000];
	bool have_that = false;
	while (fgets(ssss, sizeof(ssss), ptr) != NULL)
	{
		remove_the_enter(ssss);
		if (strcmp(ssss, argv[2]) == 0)
		{
			have_that = true;
			break;
		}
	}
	fclose(ptr);
	if (!have_that)
	{
		printf("branch does not exist!\n");
		return;
	}
	get_repo_add();
	tmpp[1000];
	strcpy(tmpp, repo_add);
	strcat(tmpp, "/.neogit/staging");
	if (!file_is_empty(tmpp))
	{
		printf("staging area is not empty!\n");
		return;
	}
	char add_fat[1000];
	strcpy(add_fat, repo_add);
	strcat(add_fat, "/.neogit/last_seen_commit");
	FILE *ptr_fattar = fopen(add_fat, "r");
	char ssstrrr[100];
	fgets(ssstrrr, sizeof(ssstrrr), ptr_fattar);
	fclose(ptr_fattar);
	remove_the_enter(ssstrrr);
	int last_seen_com = -1;
	sscanf(ssstrrr, "%d", &last_seen_com);
	if ((!all_changes_are_committed()) && last_seen_com == get_last_commit_id())
	{
		printf("some changes have not been committed!\n");
		return;
	}
	strcpy(tmpp, repo_add);
	strcat(tmpp, "/.neogit/current_branch");
	ptr = fopen(tmpp, "w");
	fprintf(ptr, "%s", argv[2]);
	fclose(ptr);
	empty_add_history();
	empty_reset_history();
	printf("branch changed successfully!\n");
	return;
}

void print_file_status(char *add)
{
	int in_stage = -1;
	if (is_in_staging_area(add)) in_stage = 1;
	else in_stage = 0;
	int com_id = get_last_commit_id();
	get_repo_add();
	char tmpp[1000];
	strcpy(tmpp, repo_add);
	strcat(tmpp, "/.neogit/commits/");
	int xxx = com_id;
	int nnnn = strlen(tmpp);
	if (xxx == 0) tmpp[nnnn++] = '0';
	while (xxx > 0)
	{
		tmpp[nnnn++] = xxx % 10 + '0';
		xxx /= 10;
	}
	tmpp[nnnn] = '\0';
	strcat(tmpp, "/files");
	FILE *ptr = fopen(tmpp, "r");
	char ssss[1000], add_copy[1000], what_name[1000];
	int indx = -1;
	nnnn = strlen(add);
	for (int i = nnnn - 1; i >= 0; i--)
	{
		if (add[i] == '/')
		{
			indx = i;
			break;
		}
	}
	strcpy(what_name, add + indx + 1);
	strcpy(add_copy, add);
	change_add_to_name(add_copy);
	int cnt_have = get_number_of_versions(add_copy);
	bool have_it = false;
	while (fgets(ssss, sizeof(ssss), ptr) != NULL)
	{
		remove_the_enter(ssss);
		if (version_of_file(ssss, add_copy, cnt_have + 3) != -1)
		{
			have_it = true;
			break;
		}
	}
	fclose(ptr);
	if (!have_it)
	{
		if (in_stage) printf("+");
		else printf("-");
		printf("A %s (%s)\n", what_name, add);
		return;
	}
	char bad_add[1000];
	strcpy(bad_add, repo_add);
	strcat(bad_add, "/.neogit/");
	strcat(bad_add, ssss);
	if (files_are_equal(bad_add, add)) return;
	if (in_stage) printf("+");
	else printf("-");
	printf("M %s (%s)\n", what_name, add);
	return;
}

void run_status()
{
	get_repo_add();
	get_all_repo_files();
	int com_id = get_last_commit_id();
	char tmpp[1000];
	strcpy(tmpp, repo_add);
	strcat(tmpp, "/.neogit/commits/");
	int xxx = com_id;
	int nnnn = strlen(tmpp);
	if (xxx == 0) tmpp[nnnn++] = '0';
	while (xxx > 0)
	{
		tmpp[nnnn++] = xxx % 10 + '0';
		xxx /= 10;
	}
	tmpp[nnnn] = '\0';
	strcat(tmpp, "/files");
	copy_file_into_array(tmpp);
	for (int i = 0; i < size_of_copied_file_array; i++) copied_file_array_ok[i] = 0;
	char bad_str[1000];
	for (int i = 0; i < number_of_all_repo_files; i++)
	{
		strcpy(bad_str, all_repo_files[i]);
		change_add_to_name(bad_str);
		int cnt_bad = get_number_of_versions(bad_str);
		for (int j = 0; j < size_of_copied_file_array; j++)
		{
			if (version_of_file(copied_file_array[j], bad_str, cnt_bad + 3) != -1)
			{
				copied_file_array_ok[j] = 1;
				break;
			}
		}
		print_file_status(all_repo_files[i]);
	}
	char what_name[1000];
	for (int i = 0; i < size_of_copied_file_array; i++)
	{
		if (copied_file_array_ok[i]) continue;
		strcpy(bad_str, copied_file_array[i]);
		change_name_to_add(bad_str);
		int indx = -1;
		int nnnn = strlen(bad_str);
		for (int i = nnnn - 1; i >= 0; i--)
		{
			if (bad_str[i] == '/')
			{
				indx = i; break;
			}
		}
		bad_str[indx] = '\0';
		if (is_in_staging_area(bad_str)) printf("+");
		else printf("-");
		nnnn = strlen(bad_str);
		for (int i = nnnn - 1; i >= 0; i--)
		{
			if (bad_str[i] == '/')
			{
				indx = i; break;
			}
		}
		strcpy(what_name, bad_str + indx + 1);
		printf("D %s (%s)\n", what_name, bad_str);
	}
	return;
}

void reset_one_file(char *add)
{
	get_repo_add();
	char tmpp[1000], tmpp2[1000], tmpp3[1000], bad_tmpp[1000];
	strcpy(bad_tmpp, repo_add);
	strcat(bad_tmpp, "/.neogit/reset_history");
	FILE *ptr_bad_ptr = fopen(bad_tmpp, "a");
	fprintf(ptr_bad_ptr, "%s\n", add);
	fclose(ptr_bad_ptr);
	strcpy(tmpp, add);
	change_add_to_name(tmpp);
	int cnt_have = get_number_of_versions(tmpp);
	strcpy(tmpp2, repo_add);
	strcat(tmpp2, "/.neogit/staging");
	strcpy(tmpp3, repo_add);
	strcat(tmpp3, "/.neogit/tmp_file");
	FILE *ptr = fopen(tmpp2, "r"), *ptr2 = fopen(tmpp3, "w");
	char ssss[1000];
	while (fgets(ssss, sizeof(ssss), ptr) != NULL)
	{
		bool have_enter = false;
		int nnnn = strlen(ssss);
		if (ssss[nnnn - 1] == '\n')
		{
			ssss[nnnn - 1] = '\0';
			have_enter = true;
		}
		if (version_of_file(ssss, tmpp, cnt_have + 3) == -1)
		{
			if (have_enter) ssss[nnnn - 1] = '\n';
			fprintf(ptr2, "%s", ssss);
		}
	}
	fclose(ptr); fclose(ptr2);
	remove(tmpp2);
	rename(tmpp3, tmpp2);
	return;
}

void reset_one_folder(char *add)
{
	DIR *dir = opendir(add);
	struct dirent *entry;
	char tmpp[1000];
	while ((entry = readdir(dir)) != NULL)
	{
		if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0 || strcmp(entry->d_name, ".neogit") == 0) continue;
		strcpy(tmpp, add);
		strcat(tmpp, "/");
		strcat(tmpp, entry->d_name);
		if (entry->d_type == DT_REG)
		{
			reset_one_file(tmpp);
			continue;
		}
		if (entry->d_type == DT_DIR)
		{
			reset_one_folder(tmpp);
		}
	}
	return;
}


void run_reset(int argc, char *argv[]) // -undo not handled
{
	int ind = 2;
	if (argc <= 2)
	{
		printf("please enter a valid command!\n");
		return;
	}
	if (strcmp(argv[2], "-undo") == 0)
	{
		if (argc != 3)
		{
			printf("please enter a valid command!\n");
			return;
		}
		run_reset_undo();
		return;
	}
	if (strcmp(argv[2], "-f") == 0)
	{
		ind++;
		if (argc <= 3)
		{
			printf("please enter a valid command!\n");
			return;
		}
	}
	while (ind < argc)
	{
		DIR *dir = opendir(".");
		struct dirent *entry;
		char tmpp[1000];
		while ((entry = readdir(dir)) != NULL)
		{
			if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0 || strcmp(entry->d_name, ".neogit") == 0) continue;
			if (name_equal_to_wild(entry->d_name, argv[ind]))
			{
				getcwd(tmpp, sizeof(tmpp));
				strcat(tmpp, "/");
				strcat(tmpp, entry->d_name);
				if (entry->d_type == DT_REG)
				{
					reset_one_file(tmpp);
					continue;
				}
				if (entry->d_type == DT_DIR)
				{
					reset_one_folder(tmpp);
				}
			}
		}
		ind++;
	}
	return;
}

void run_reset_undo()
{
	get_repo_add();
	char tmpp[1000], ssss[1000];
	strcpy(tmpp, repo_add);
	strcat(tmpp, "/.neogit/add_history_cnt");
	FILE *ptr = fopen(tmpp, "r");
	int add_cnt = -1;
	fgets(ssss, sizeof(ssss), ptr);
	remove_the_enter(ssss);
	fclose(ptr);
	sscanf(ssss, "%d", &add_cnt);
	if (add_cnt == 0)
	{
		printf("add history is empty.\nno add command to undo!\n");
		return;
	}
	ptr = fopen(tmpp, "w");
	fprintf(ptr, "%d", add_cnt - 1);
	fclose(ptr);
	strcpy(tmpp, repo_add);
	strcat(tmpp, "/.neogit/add_history/");
	int xxx = add_cnt;
	int nnnn = strlen(tmpp);
	if (xxx == 0) tmpp[nnnn++] = '0';
	while (xxx > 0)
	{
		tmpp[nnnn++] = xxx % 10 + '0';
		xxx /= 10;
	}
	tmpp[nnnn] = '\0';
	ptr = fopen(tmpp, "r");
	while (fgets(ssss, sizeof(ssss), ptr) != NULL)
	{
		remove_the_enter(ssss);
		reset_one_file(ssss);
	}
	fclose(ptr);
	remove(tmpp);
	return;
}

void restore_the_alias(int *argc, char *argv[], char mess[], int ind)
{
	char *tok = strtok(mess, " ");
	while (tok != NULL)
	{
		shift_to_right(argv, ind, *argc);
		(*argc)++;
		strcpy(argv[ind], tok);
		ind++;
		tok = strtok(NULL, " ");
	}
	return;
}

void add_aliases_of_file(int *argc, char *argv[], char *add) // receives the address of folder which has alias_folder
{
	char bad_add[1000], bad_add2[1000], ssss1[1000], ssss2[1000];
	int ind = (*argc) - 1;
	while (ind >= 0)
	{
		strcpy(bad_add, add);
		strcpy(bad_add2, add);
		strcat(bad_add, "/alias/first");
		strcat(bad_add2, "/alias/second");
		FILE *ptr1 = fopen(bad_add, "r"), *ptr2 = fopen(bad_add2, "r");
		bool have_it = false;
		while (fgets(ssss1, sizeof(ssss1), ptr1) != NULL)
		{
			fgets(ssss2, sizeof(ssss2), ptr2);
			remove_the_enter(ssss1); remove_the_enter(ssss2);
			if (strcmp(argv[ind], ssss1) == 0) 
			{
				have_it = true;
				break;
			}
		}
		fclose(ptr1); fclose(ptr2);
		if (have_it)
		{
			shift_to_left(argv, ind, (*argc));
			(*argc)--;
			restore_the_alias(argc, argv, ssss2, ind);
		}
		ind--;
	}
	return;
}

void run_set_short(int argc, char *argv[])
{
	if (argc != 6)
	{
		printf("please enter a valid command!\n");
		return;
	}
	if (strcmp(argv[2], "-m") != 0)
	{
		printf("please enter a valid command!\n");
		return;
	}
	if (strcmp(argv[4], "-s") != 0)
	{
		printf("please enter a valid command!\n");
		return;
	}
	get_repo_add();
	char tmpp[1000], tmpp2[1000], ssss[1000];
	strcpy(tmpp, repo_add);
	strcat(tmpp, "/.neogit/shortcuts/first");
	FILE *ptr1 = fopen(tmpp, "r");
	bool have_it = false;
	while (fgets(ssss, sizeof(ssss), ptr1) != NULL)
	{
		remove_the_enter(ssss);
		if (strcmp(ssss, argv[5]) == 0)
		{
			have_it = true;
			break;
		}
	}
	fclose(ptr1);
	if (have_it)
	{
		printf("shortcut-name already exists!\n");
		return;
	}
	ptr1 = fopen(tmpp, "a");
	strcpy(tmpp2, repo_add);
	strcat(tmpp2, "/.neogit/shortcuts/second");
	FILE *ptr2 = fopen(tmpp2, "a");
	fprintf(ptr1, "%s\n", argv[5]);
	fprintf(ptr2, "%s\n", argv[3]);
	fclose(ptr1); fclose(ptr2);
	return;
}

void run_replace_short(int argc, char *argv[])
{
	if (argc != 6)
	{
		printf("please enter a valid command!\n");
		return;
	}
	if (strcmp(argv[2], "-m") != 0)
	{
		printf("please enter a valid command!\n");
		return;
	}
	if (strcmp(argv[4], "-s") != 0)
	{
		printf("please enter a valid command!\n");
		return;
	}
	get_repo_add();
	char tmpp[1000], tmpp2[1000], tmpp3[1000], ssss[1000];
	strcpy(tmpp, repo_add);
	strcat(tmpp, "/.neogit/shortcuts/first");
	strcpy(tmpp2, repo_add);
	strcat(tmpp2, "/.neogit/shortcuts/second");
	strcpy(tmpp3, repo_add);
	strcat(tmpp3, "/.neogit/shortcuts/tmp_file");
	FILE *ptr1 = fopen(tmpp, "r");
	int ttt = 0;
	bool have_it = false;
	while (fgets(ssss, sizeof(ssss), ptr1) != NULL)
	{
		remove_the_enter(ssss);
		if (strcmp(ssss, argv[5]) == 0)
		{
			have_it = true;
			break;
		}
		ttt++;
	}
	fclose(ptr1);
	if (!have_it)
	{
		printf("shortcut-name does not exist!\n");
		return;
	}
	ptr1 = fopen(tmpp2, "r");
	FILE *ptr2 = fopen(tmpp3, "w");
	int ttt2 = 0;
	while (fgets(ssss, sizeof(ssss), ptr1) != NULL)
	{
		bool have_enter = false;
		int nnnn = strlen(ssss);
		if (ssss[nnnn - 1] == '\n') 
		{
			ssss[nnnn - 1] = '\0';
			have_enter = true;
		}
		if (ttt == ttt2)
		{
			fprintf(ptr2, "%s\n", argv[3]);
		}
		else
		{
			if (have_enter) ssss[nnnn - 1] = '\n';
			fprintf(ptr2, "%s", ssss);
		}
		ttt2++;
	}
	fclose(ptr1); fclose(ptr2);
	remove(tmpp2);
	rename(tmpp3, tmpp2);
	return;
}

void run_remove_short(int argc, char *argv[])
{
	if (argc != 4)
	{
		printf("please enter a valid command!\n");
		return;
	}
	if (strcmp(argv[2], "-s") != 0)
	{
		printf("please enter a valid command!\n");
		return;
	}
	get_repo_add();
	char tmpp[1000], tmpp2[1000], tmpp3[1000], ssss[1000];
	strcpy(tmpp, repo_add);
	strcat(tmpp, "/.neogit/shortcuts/first");
	strcpy(tmpp2, repo_add);
	strcat(tmpp2, "/.neogit/shortcuts/second");
	strcpy(tmpp3, repo_add);
	strcat(tmpp3, "/.neogit/shortcuts/tmp_file");
	FILE *ptr1 = fopen(tmpp, "r");
	int ttt = 0;
	bool have_it = false;
	while (fgets(ssss, sizeof(ssss), ptr1) != NULL)
	{
		remove_the_enter(ssss);
		if (strcmp(ssss, argv[3]) == 0)
		{
			have_it = true;
			break;
		}
		ttt++;
	}
	fclose(ptr1);
	if (!have_it)
	{
		printf("shortcut-name does not exist!\n");
		return;
	}
	ptr1 = fopen(tmpp2, "r");
	FILE *ptr2 = fopen(tmpp3, "w");
	int ttt2 = 0;
	while (fgets(ssss, sizeof(ssss), ptr1) != NULL)
	{
		if (ttt != ttt2) fprintf(ptr2, "%s", ssss);
		ttt2++;
	}
	fclose(ptr1); fclose(ptr2);
	remove(tmpp2);
	rename(tmpp3, tmpp2);
	ptr1 = fopen(tmpp, "r");
	ptr2 = fopen(tmpp3, "w");
	ttt2 = 0;
	while (fgets(ssss, sizeof(ssss), ptr1) != NULL)
	{
		if (ttt != ttt2) fprintf(ptr2, "%s", ssss);
		ttt2++;
	}
	fclose(ptr1); fclose(ptr2);
	remove(tmpp);
	rename(tmpp3, tmpp);
	return;

}

void get_commit_info_line(char *ss, int com_id, int line_num)
{
	get_repo_add();
	char tmpp[1000];
	strcpy(tmpp, repo_add);
	strcat(tmpp, "/.neogit/commits/");
	int xxx = com_id;
	int nnnn = strlen(tmpp);
	if (xxx == 0) tmpp[nnnn++] = '0';
	while (xxx > 0)
	{
		tmpp[nnnn++] = xxx % 10 + '0';
		xxx /= 10;
	}
	tmpp[nnnn] = '\0';
	strcat(tmpp, "/info");
	FILE *ptr = fopen(tmpp, "r");
	char ssss[1000];
	for (int i = 1; i <= line_num; i++) fgets(ssss, sizeof(ssss), ptr);
	remove_the_enter(ssss);
	strcpy(ss, ssss);
	return;
}

void print_commit_info(int com_id)
{
	get_repo_add();
	char tmpp[1000];
	strcpy(tmpp, repo_add);
	strcat(tmpp, "/.neogit/commits/");
	int xxx = com_id;
	int nnnn = strlen(tmpp);
	if (xxx == 0) tmpp[nnnn++] = '0';
	while (xxx > 0)
	{
		tmpp[nnnn++] = xxx % 10 + '0';
		xxx /= 10;
	}
	tmpp[nnnn] = '\0';
	strcat(tmpp, "/info");
	FILE *ptr = fopen(tmpp, "r");
	char ssss[1000];
	int num = -1;
	fgets(ssss, sizeof(ssss), ptr);
	remove_the_enter(ssss);
	sscanf(ssss, "%d", &num);
	printf("commit id: %d\n", num);
	fgets(ssss, sizeof(ssss), ptr);
	remove_the_enter(ssss);
	printf("branch: %s\n", ssss);
	fgets(ssss, sizeof(ssss), ptr);
	remove_the_enter(ssss);
	fgets(ssss, sizeof(ssss), ptr);
	remove_the_enter(ssss);
	printf("date and time: %s\n", ssss);
	fgets(ssss, sizeof(ssss), ptr);
	remove_the_enter(ssss);
	printf("author: %s <", ssss);
	fgets(ssss, sizeof(ssss), ptr);
	remove_the_enter(ssss);
	printf("%s>\n", ssss);
	fgets(ssss, sizeof(ssss), ptr);
	remove_the_enter(ssss);
	printf("commit message: %s\n", ssss);
	fgets(ssss, sizeof(ssss), ptr);
	remove_the_enter(ssss);
	sscanf(ssss, "%d", &num);
	printf("number of committed files: %d\n", num);
	return;
}

int get_parent_of_commit(int com_id)
{
	char ssss[1000];
	get_commit_info_line(ssss, com_id, 3);
	int num = -1;
	sscanf(ssss, "%d", &num);
	return num;
}

void run_log(int argc, char *argv[])
{
	int commits_num = get_number_of_all_commits();
	if (argc == 2)
	{
		for (int i = commits_num; i >= 1; i--) print_commit_info(i);
		return;
	}
	if (argc <= 2)
	{
		printf("please enter a valid commmand!\n");
		return;
	}
	if (strcmp(argv[2], "-n") == 0)
	{
		if (argc != 4)
		{
			printf("please enter a valid command!\n");
			return;
		}
		int ccnt = -1, num = -1;
		sscanf(argv[3], "%d%n", &num, &ccnt);
		if (ccnt != strlen(argv[3]) || num <= 0 || num > commits_num)
		{
			printf("please enter a valid number!\n");
			return;
		}
		int tt = commits_num;
		for (int i = 1; i <= num; i++) print_commit_info(tt--);
		return;
	}
	if (strcmp(argv[2], "-author") == 0)
	{
		if (argc != 4)
		{
			printf("please enter a valid command!\n");
			return;
		}
		char ssss[1000];
		for (int i = commits_num; i >= 1; i--)
		{
			get_commit_info_line(ssss, i, 5);
			if (strcmp(argv[3], ssss) == 0) print_commit_info(i);
		}
		return;
	}
	if (strcmp(argv[2], "-branch") == 0)
	{
		if (argc != 4)
		{
			printf("please enter a valid command!\n");
			return;
		}
		get_repo_add();
		char tmpp[1000];
		strcpy(tmpp, repo_add);
		strcat(tmpp, "/.neogit/branch_names");
		char ssss[1000];
		FILE *ptr = fopen(tmpp, "r");
		bool have_it = false;
		while (fgets(ssss, sizeof(ssss), ptr) != NULL)
		{
			remove_the_enter(ssss);
			if (strcmp(ssss, argv[3]) == 0)
			{
				have_it = true;
				break;
			}
		}
		fclose(ptr);
		if (!have_it)
		{
			printf("branch name does not exist!\n");
			return;
		}
		for (int i = commits_num; i >= 1; i--)
		{
			get_commit_info_line(ssss, i, 2);
			if (strcmp(ssss, argv[3]) == 0) print_commit_info(i);
		}
		return;
	}
	if (strcmp(argv[2], "-since") == 0 || strcmp(argv[2], "-before") == 0)
	{
		if (argc != 6)
		{
			printf("please enter a valid command!\n");
			return;
		}
		int ccnt = -1, d1 = -1, m1 = -1, y1 = -1;
		sscanf(argv[3], "%d%n", &d1, &ccnt);
		if (ccnt != strlen(argv[3]) || d1 <= 0 || d1 >= 32)
		{
			printf("please enter a valid date!\n");
			return;
		}
		m1 = get_number_of_month(argv[4]);
		if (m1 == -1)
		{
			printf("please enter a valid date!\n");
			return;
		}
		ccnt = -1;
		sscanf(argv[5], "%d%n", &y1, &ccnt);
		if (ccnt != strlen(argv[5]) || y1 <= 0)
		{
			printf("please enter a valid date!\n");
			return;
		}
		char ssss[1000];
		for (int i = commits_num; i >= 1; i--)
		{
			int d2 = -1, m2 = -1, y2 = -1;
			get_commit_info_line(ssss, i, 4);
			char *tok = strtok(ssss, " ");
			tok = strtok(NULL, " ");
			m2 = get_number_of_month(tok);
			tok = strtok(NULL, " ");
			sscanf(tok, "%d", &d2);
			tok = strtok(NULL, " ");
			tok = strtok(NULL, " ");
			sscanf(tok, "%d", &y2);
			if (strcmp(argv[2], "-before") == 0 && date_bigger_than_date(d1, m1, y1, d2, m2, y2)) print_commit_info(i);
			if (strcmp(argv[2], "-since") == 0 && date_bigger_than_date(d2, m2, y2, d1, m1, y1)) print_commit_info(i);
		}
		return;
	}
	if (strcmp(argv[2], "-search") != 0)
	{
		printf("please enter a valid command!\n");
	}
	if (argc <= 3)
	{
		printf("please enter a valid command!\n");
		return;
	}
	char ssss[1000];
	for (int i = commits_num; i >= 1; i--)
	{
		get_commit_info_line(ssss, i, 7);
		char *tok = strtok(ssss, " ");
		bool have_it = false;
		while (tok != NULL)
		{
			for (int i = 3; i < argc; i++)
			{
				if (name_equal_to_wild(tok, argv[i]))
				{
					have_it = true;
					break;
				}
			}
			tok = strtok(NULL, " ");
			if (have_it) break;
		}
		if (have_it) print_commit_info(i);
	}
	return;
}

void print_all_tag_names()
{
	get_repo_add();
	char tmpp[1000];
	strcpy(tmpp, repo_add);
	strcat(tmpp, "/.neogit/tag_names");
	copy_file_into_array(tmpp);
	int sz = size_of_copied_file_array;
	char ssss[1000];
	for (int tt = sz - 1; tt >= 1; tt--)
	{
		for (int i = 0; i < tt; i++)
		{
			if (strcmp(copied_file_array[i], copied_file_array[i + 1]) > 0)
			{
				strcpy(ssss, copied_file_array[i]);
				strcpy(copied_file_array[i], copied_file_array[i + 1]);
				strcpy(copied_file_array[i + 1], ssss);
			}
		}
	}
	for (int i = 0; i < sz; i++) printf("%s\n", copied_file_array[i]);
	return;
}

void run_tag(int argc, char *argv[])
{
	if (argc == 2)
	{
		print_all_tag_names();
		return;
	}
	if (strcmp(argv[2], "-a") == 0)
	{
		char mess[1000];
		mess[0] = '\0';
		bool have_mess = false;
		int com_id = get_last_commit_id();
		int ind = 4;
		bool have_f = false;
		while (ind < argc)
		{
			if (strcmp(argv[ind], "-m") == 0)
			{
				strcpy(mess, argv[ind + 1]);
				have_mess = true;
				ind += 2;
				continue;
			}
			if (strcmp(argv[ind], "-c") == 0)
			{
				int num = -1;
				sscanf(argv[ind + 1], "%d", &num);
				ind += 2;
				com_id = num;
				continue;
			}
			if (strcmp(argv[ind], "-f") == 0) 
			{
				have_f = true;
				ind++;
			}
		}
		bool have_it = false;
		char ssss[1000];
		get_repo_add();
		char tmpp[1000];
		strcpy(tmpp, repo_add);
		strcat(tmpp, "/.neogit/tag_names");
		FILE *ptr = fopen(tmpp, "r");
		while (fgets(ssss, sizeof(ssss), ptr) != NULL)
		{
			remove_the_enter(ssss);
			if (strcmp(ssss, argv[3]) == 0)
			{
				have_it = true;
				break;
			}
		}
		fclose(ptr);
		if (!have_f)
		{
			if (have_it)
			{
				printf("tag-name already exists!\n");
				return;
			}
		}
		if (!have_it)
		{
			strcpy(tmpp, repo_add);
			strcat(tmpp, "/.neogit/tag_names");
			ptr = fopen(tmpp, "a");
			fprintf(ptr, "%s\n", argv[3]);
			fclose(ptr);
		}
		strcpy(tmpp, repo_add);
		strcat(tmpp, "/.neogit/tags/");
		strcat(tmpp, argv[3]);
		ptr = fopen(tmpp, "w");
		fprintf(ptr, "tag %s\n", argv[3]);
		fprintf(ptr, "commit %d\n", com_id);
		char what_name[1000], what_email[1000], what_time[1000];
		get_author_name(what_name); get_author_email(what_email); get_the_time(what_time);
		fprintf(ptr, "author: %s <%s>\n", what_name, what_email);
		fprintf(ptr, "date and time: %s\n", what_time);
		if (have_mess) fprintf(ptr, "message: %s\n", mess);
		else fprintf(ptr, "-1");
		fclose(ptr);
		return;
	}
	if (strcmp(argv[2], "show") != 0)
	{
		printf("please enter a valid command!\n");
		return;
	}
	if (argc != 4)
	{
		printf("please enter a valid command!\n");
		return;
	}
	get_repo_add();
	char tmpp[1000];
	strcpy(tmpp, repo_add);
	strcat(tmpp, "/.neogit/tag_names");
	FILE *ptr = fopen(tmpp, "r");
	char ssss[1000];
	bool have_it = false;
	while (fgets(ssss, sizeof(ssss), ptr) != NULL)
	{
		remove_the_enter(ssss);
		if (strcmp(ssss, argv[3]) == 0)
		{
			have_it = true;
			break;
		}
	}
	fclose(ptr);
	if (!have_it)
	{
		printf("tag-name does not exist!\n");
		return;
	}
	strcpy(tmpp, repo_add);
	strcat(tmpp, "/.neogit/tags/");
	strcat(tmpp, argv[3]);
	ptr = fopen(tmpp, "r");
	for (int i = 0; i < 4; i++)
	{
		fgets(ssss, sizeof(ssss), ptr);
		printf("%s", ssss);
	}
	fgets(ssss, sizeof(ssss), ptr);
	remove_the_enter(ssss);
	if (strcmp(ssss, "-1") != 0) printf("%s\n", ssss);
	else printf("no message\n");
	return;
}

int get_todo_check(char *add)
{
	char tmp_for[1000];
	get_file_format_bad(tmp_for, add);
	if (strcmp(tmp_for, ".cpp") != 0 && strcmp(tmp_for, ".c") != 0 && strcmp(tmp_for, ".txt") != 0)
	{
		return -1;
	}
	char ssss[1000];
	FILE *ptr = fopen(add, "r");
	bool have_it = false;
	while (fgets(ssss, sizeof(ssss), ptr) != NULL)
	{
		int nnnn = strlen(ssss);
		for (int i = 0; i < nnnn; i++)
		{
			if (is_white(ssss[i])) ssss[i] = ' ';
		}
		char *tok = strtok(ssss, " ");
		while (tok != NULL)
		{
			if (strcmp(tok, "TODO") == 0)
			{
				have_it = true;
			}
			tok = strtok(NULL, " ");
		}
		if (have_it) break;
	}
	fclose(ptr);
	if (strcmp(tmp_for, ".txt") == 0)
	{
		if (!have_it) return 1;
		return 0;
	}
	ptr = fopen(add, "r");
	have_it = false;
	while (fgets(ssss, sizeof(ssss), ptr) != NULL)
	{
		int nnnn = strlen(ssss);
		char *ccc = ssss;
		ccc = strstr(ccc, "TODO");
		while (ccc != NULL)
		{
			int ind = ccc - ssss;
			if (ind != 0)
			{
				if (!is_white(ssss[ind - 1]))
				{
					ccc += 4;
					continue;
				}
			}
			ind += 4;
			if (ind >= nnnn) break;
			if (ssss[ind] != '(')
			{
				ccc += 4;
				continue;
			}
			bool badbad = false, have_good = false;
			for (int i = ind + 1; i < nnnn; i++)
			{
				if (ssss[i] == '\n') badbad = true;
				if (ssss[i] == ')')
				{
					have_good = true;
					break;
				}
			}
			if (have_good && (!badbad))
			{
				have_it = true;
				break;
			}
			ccc += 4;
		}
		if (have_it) break;
	}
	fclose(ptr);
	if (!have_it) return 1;
	return 0;
}

int get_eof_blank_space(char *add)
{
	char tmp_for[1000];
	get_file_format_bad(tmp_for, add);
	if (strcmp(tmp_for, ".cpp") != 0 && strcmp(tmp_for, ".c") != 0 && strcmp(tmp_for, ".txt") != 0)
	{
		return -1;
	}
	FILE *ptr = fopen(add, "r");
	char ssss[1000];
	bool is_ok = true;
	int ttt = 0, cnt_bad = get_number_of_lines(add);
	while (fgets(ssss, sizeof(ssss), ptr) != NULL)
	{
		int nnnn = strlen(ssss);
		ttt++;
		if (ttt == cnt_bad) nnnn--;
		if (nnnn >= 1)
		{
			if (is_white(ssss[nnnn - 1])) is_ok = false;
			else is_ok = true;
		}
	}
	fclose(ptr);
	if (is_ok) return 1;
	return 0;
}

int get_character_limit(char *add) // not checked_yet!
{
	char tmp_for[1000];
	get_file_format_bad(tmp_for, add);
	if (strcmp(tmp_for, ".cpp") != 0 && strcmp(tmp_for, ".c") != 0 && strcmp(tmp_for, ".txt") != 0)
	{
		return -1;
	}
	long long int cnt = 0;
	FILE *ptr = fopen(add, "r");
	char ssss[1000];
	while (fgets(ssss, sizeof(ssss), ptr) != NULL)
	{
		cnt += strlen(ssss);
	}
	fclose(ptr);
	if (cnt > 20000) return 0;
	return 1;
}

void run_pre_commit(int argc, char *argv[])
{
	get_repo_add();
	if (argc == 2)
	{
		char tmpp[1000];
		strcpy(tmpp, repo_add);
		strcat(tmpp, "/.neogit/staging");
		FILE *ptr_staging = fopen(tmpp, "r");
		char ssss[1000], prec_add[1000], prec_com[1000], bad_add[1000];
		strcpy(prec_add, repo_add);
		strcat(prec_add, "/.neogit/applied_hooks");
		while (fgets(ssss, sizeof(ssss), ptr_staging) != NULL)
		{
			remove_the_enter(ssss);
			FILE *ptr_prec = fopen(prec_add, "r");
			int nnnn = strlen(ssss);
			int ind1 = -1;
			strcpy(bad_add, repo_add);
			strcat(bad_add, "/.neogit/");
			strcat(bad_add, ssss);
			for (int i = nnnn - 1; i >= 0; i--)
			{
				if (ssss[i] == '#')
				{
					ind1 = i;
					break;
				}
			}
			int ind2 = -1;
			for (int i = ind1 - 1; i >= 0; i--)
			{
				if (ssss[i] == '#')
				{
					ind2 = i;
					break;
				}
			}
			for (int i = ind2 + 1; i < ind1; i++) printf("%c", ssss[i]);
			printf(":\n");
			while (fgets(prec_com, sizeof(prec_com), ptr_prec) != NULL)
			{
				remove_the_enter(prec_com);
				int ans = -2;
				if (strcmp(prec_com, "todo-check") == 0) ans = get_todo_check(bad_add);
				if (strcmp(prec_com, "eof-blank-space") == 0) ans = get_eof_blank_space(bad_add);
				if (strcmp(prec_com, "character-limit") == 0) ans = get_character_limit(bad_add);
				printf("%s", prec_com);
				int bbb = 0;
				if (ans == -1) bbb = 1;
				for (int i = 0; i < 30 - strlen(prec_com) - bbb; i++) printf(".");
				if (ans == -1) printf("\033[1;37mSKIPPED\033[1;0m\n");
				if (ans == 0) printf("\033[1;31mFAILED\033[1;0m\n");
				if (ans == 1) printf("\033[1;32mPASSED\033[1;0m\n");
			}
			fclose(ptr_prec);
		}
		fclose(ptr_staging);
		return;
	}
	char tmpp[1000];
	if (strcmp(argv[2], "hooks") == 0 && strcmp(argv[3], "list") == 0)
	{
		if (argc != 4)
		{
			printf("please enter a valid command!\n");
			return;
		}
		strcpy(tmpp, repo_add);
		strcat(tmpp, "/.neogit/hooks_list");
		print_one_file_info(tmpp);
		return;
	}
	if (strcmp(argv[2], "applied") == 0 && strcmp(argv[3], "hooks") == 0)
	{
		if (argc != 4)
		{
			printf("please enter a valid command!\n");
			return;
		}
		strcpy(tmpp, repo_add);
		strcat(tmpp, "/.neogit/applied_hooks");
		print_one_file_info(tmpp);
		return;
	}
	if (strcmp(argv[2], "add") == 0)
	{
		if (argc != 5)
		{
			printf("please enter a valid command!\n");
			return;
		}
		if (strcmp(argv[3], "hook") != 0)
		{
			printf("please enter a valid command!\n");
			return;
		}
		strcpy(tmpp, repo_add);
		strcat(tmpp, "/.neogit/applied_hooks");
		if (have_line_in_file(tmpp, argv[4]))
		{
			printf("hook already exists!\n");
			return;
		}
		add_line_to_file(tmpp, argv[4]);
		return;
	}
	if (strcmp(argv[2], "remove") == 0)
	{
		if (argc != 5)
		{
			printf("please enter a valid command!\n");
			return;
		}
		if (strcmp(argv[3], "hook") != 0)
		{
			printf("please enter a valid command!\n");
			return;
		}
		strcpy(tmpp, repo_add);
		strcat(tmpp, "/.neogit/applied_hooks");
		if (!have_line_in_file(tmpp, argv[4]))
		{
			printf("hook does not exist!\n");
			return;
		}
		remove_line_from_file(tmpp, argv[4]);
		return;
	}
}

int main(int argc, char *argv[])
{
	// just use argv2 !!!!

	argv2 = (char**) malloc(2000 * sizeof(char*));
	for (int i = 0; i < 2000; i++) argv2[i] = (char*) malloc(2000 * sizeof(char));
	for (int i = 0; i < argc; i++) strcpy(argv2[i], argv[i]);
	add_aliases_of_file(&argc, argv2, "/home/parsa/.neogit_prop");
	get_repo_add();
	if (strlen(repo_add) != 0)
	{
		char tmpp[1000];
		strcpy(tmpp, repo_add);
		strcat(tmpp, "/.neogit");
		add_aliases_of_file(&argc, argv2, tmpp);
	}

	//for (int i = 0; i < argc; i++) printf("<%s>\n", argv2[i]);

	/*
	   check all_changes_are_committed()
	printf(" %d\n", all_changes_are_committed());
	for (int i = 0; i < number_of_all_repo_files; i++) printf("<%s>\n", all_repo_files[i]);
	printf("-----\n");
	for (int i = 0; i < size_of_copied_file_array; i++) printf("<%s>\n", copied_file_array[i]);
	printf("-----\n");
	*/
	//printf("%d\n", get_eof_blank_space(argv[1]));
	Flag = false;
	if (argc <= 1)
	{
		printf("please enter a valid command!\n");
		return 0;
	}
	if (strcmp(argv2[1], "config") == 0)
	{
		run_config(argc, argv2);
		return 0;
	}
	
	if (strcmp(argv2[1], "init") == 0)
	{
		run_init(argc, argv2);
		return 0;
	}
	if (strcmp(argv2[1], "add") == 0)
	{
		get_repo_add();
		if (strlen(repo_add) == 0)
		{
			printf("please create repo first!\n");
			return 0;
		}
		run_add(argc, argv2);
		return 0;
	}
	if (strcmp(argv2[1], "commit") == 0)
	{
		get_repo_add();
		if (strlen(repo_add) == 0)
		{
			printf("please create repo first!\n");
			return 0;
		}
		run_commit(argc, argv2);
		return 0;
	}
	if (strcmp(argv2[1], "branch") == 0)
	{
		get_repo_add();
		if (strlen(repo_add) == 0)
		{
			printf("please create repo first!\n");
			return 0;
		}
		run_branch(argc, argv2);
		return 0;
	}
	if (strcmp(argv2[1], "checkout") == 0)
	{
		get_repo_add();
		if (strlen(repo_add) == 0)
		{
			printf("please create repo first!\n");
			return 0;
		}
		run_checkout(argc, argv2);
		return 0;
	}
	if (strcmp(argv2[1], "status") == 0)
	{
		get_repo_add();
		if (strlen(repo_add) == 0)
		{
			printf("please create repo first!\n");
			return 0;
		}
		if (argc != 2)
		{
			printf("please enter a valid command!\n");
			return 0;
		}
		run_status();
		return 0;
	}
	if (strcmp(argv2[1], "reset") == 0)
	{
		get_repo_add();
		if (strlen(repo_add) == 0)
		{
			printf("please create repo first!\n");
			return 0;
		}
		run_reset(argc, argv2);
		return 0;
	}
	if (strcmp(argv2[1], "set") == 0)
	{
		get_repo_add();
		if (strlen(repo_add) == 0)
		{
			printf("please create repo first!\n");
			return 0;
		}
		run_set_short(argc, argv2);
		return 0;
	}
	if (strcmp(argv2[1], "replace") == 0)
	{
		get_repo_add();
		if (strlen(repo_add) == 0)
		{
			printf("please create repo first!\n");
			return 0;
		}
		run_replace_short(argc, argv2);
		return 0;
	}
	if (strcmp(argv2[1], "remove") == 0)
	{
		get_repo_add();
		if (strlen(repo_add) == 0)
		{
			printf("please create repo first!\n");
			return 0;
		}
		run_remove_short(argc, argv2);
		return 0;
	}
	if (strcmp(argv2[1], "log") == 0)
	{
		get_repo_add();
		if (strlen(repo_add) == 0)
		{
			printf("please create repo first!\n");
			return 0;
		}
		run_log(argc, argv2);
		return 0;
	}
	if (strcmp(argv2[1], "tag") == 0)
	{
		get_repo_add();
		if (strlen(repo_add) == 0)
		{
			printf("please create repo first!\n");
			return 0;
		}
		run_tag(argc, argv2);
		return 0;
	}
	if (strcmp(argv2[1], "pre-commit") == 0)
	{
		get_repo_add();
		if (strlen(repo_add) == 0)
		{
			printf("please create repo first!\n");
			return 0;
		}
		run_pre_commit(argc, argv2);
		return 0;
	}
	printf("please enter a valid command!\n");
	return 0;
}
