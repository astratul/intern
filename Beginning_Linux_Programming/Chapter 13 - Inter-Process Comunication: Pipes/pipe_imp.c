#include "cd_data.h"
#include "cliserv.h"
static int server_fd = -1;
static pid_t mypid = 0;
static char client_pipe_name[PATH_MAX + 1] = {'\0'};
static int client_fd = -1;
static int client_write_fd = -1;


cdc_entry search_cdc_entry(const char *cd_catalog_ptr, int *first_call_ptr)
{
    message_db_t mess_send;
    message_db_t mess_ret;
    static FILE *work_file = (FILE *)0;
    static int entries_matching = 0;
    cdc_entry ret_val;
    ret_val.catalog[0] = '\0';
    if (!work_file && (*first_call_ptr == 0))
        return (ret_val);
    if (*first_call_ptr)
    {
        *first_call_ptr = 0;
        if (work_file)
            fclose(work_file);
        work_file = tmpfile();
        if (!work_file)
            return (ret_val);
        mess_send.client_pid = mypid;
        mess_send.request = s_find_cdc_entry;
        strcpy(mess_send.cdc_entry_data.catalog, cd_catalog_ptr);
        if (send_mess_to_server(mess_send))
        {
            if (start_resp_from_server())
            {
                while (read_resp_from_server(&mess_ret))
                {
                    if (mess_ret.response == r_success)
                    {
                        fwrite(&mess_ret.cdc_entry_data, sizeof(cdc_entry), 1, work_file);
                        entries_matching++;
                    }
                    else
                    {
                        break;
                    }
                } /* while */
            }
            else
            {
                fprintf(stderr, "Server not responding\n");
            }
        }
        else
        {
            fprintf(stderr, "Server not accepting requests\n");
        }
        if (entries_matching == 0)
        {
            fclose(work_file);
            work_file = (FILE *)0;
            return (ret_val);
        }
        (void)fseek(work_file, 0L, SEEK_SET);
    }
    else
    {
        /* not *first_call_ptr */
        if (entries_matching == 0)
        {
            fclose(work_file);
            work_file = (FILE *)0;
            return (ret_val);
        }
    }
    fread(&ret_val, sizeof(cdc_entry), 1, work_file);
    entries_matching--;
    return (ret_val);
}