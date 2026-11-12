/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: coren <coren@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/05 17:06:26 by coco              #+#    #+#             */
/*   Updated: 2026/11/12 14:46:39 by coren            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_script.h"
int		parsing(int argc, char **argv, t_script_settings *data);

#include <sys/wait.h>
int		open_all_files(t_script_data *data);
void	close_all_files(t_script_data data);
void	print_start_info(t_script_data data);
int		set_slave_path(t_script_data *data);
int		init_pty_lock(t_script_data *data);
int		save_termios_struct(t_script_data *data);
int		set_term_to_raw_mode(t_script_data *data);
int		restore_orig_term(t_script_data *data);
int		child_process(t_script_data *data);
int		handle_pty_session(t_script_data *data);
void	write_start_info(t_script_data *data);
int		update_time(t_script_data *data);
void	print_end_info(t_script_data data);
void	write_end_info(t_script_data *data);


int	script_test_exec(t_script_data	*data)
{
	if (set_slave_path(data) || init_pty_lock(data))
		return FAILED;
	
	if (save_termios_struct(data) || set_term_to_raw_mode(data))
		return FAILED;
	
	if (update_time(data))
		return FAILED;

	write_start_info(data);
	
    pid_t pid = fork();
    if (pid == -1)
        return 1; //TODO CHECK ERROR

    if (pid == 0)
	{
		if (child_process(data))
			return FAILED;
	}
    else
	{
		if (handle_pty_session(data))
			return FAILED;
	}

	int status;

	if (waitpid(pid, &status, 0) == -1)
		return FAILED;
	if (WIFEXITED(status))
    	data->childExitCode = WEXITSTATUS(status);
	else if (WIFSIGNALED(status))
   		data->childExitCode = 128 + WTERMSIG(status);

	if (restore_orig_term(data))
		return FAILED;

	return SUCCES;
}

void parse_envp(char **envp, t_script_settings *data);

int	main(int argc, char **argv, char **envp)
{
	t_script_data	data;
	ft_bzero(&data, sizeof(t_script_data));

	if (parsing(argc, argv, &(data.settings)))
		return FAILED;
	if (data.settings.isClosing)
		return SUCCES;
	parse_envp(envp, &(data.settings));

	print_start_info(data);

	if (open_all_files(&data))
	{
		close_all_files(data);	
		return FAILED;
	}
	
	if (script_test_exec(&data))
		return FAILED;

	update_time(&data);
	write_end_info(&data);
	print_end_info(data);
	
	close_all_files(data);	
	return SUCCES;
}
