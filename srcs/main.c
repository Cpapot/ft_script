/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: coren <coren@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/05 17:06:26 by coco              #+#    #+#             */
/*   Updated: 2026/10/27 11:44:46 by coren            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_script.h"

int		parsing(int argc, char **argv, t_script_settings *data);

// int	ft_script_error(char *error, int doExit)
// {
// 	write(1, ERROR_PRINT, "" )
// 	printf("%s %s", ERROR_PRINT, error);
// 	if (doExit == 1)
// 		exit(1);
// 	else
// 		return 1;
// }

//DEBUG FUCTION TO REMOVE LATER
void print_settings(t_script_settings data)
{
	ft_printf("target: %s\n", data.target);
	ft_printf("isAppend: %s\n", data.isAppend ? "true" : "false");
	ft_printf("command: %s\n", data.command);
	ft_printf("echoMode: %d\n", data.echoMode);
	ft_printf("isReturn: %s\n", data.isReturn ? "true" : "false");
	ft_printf("isFlush: %s\n", data.isFlush ? "true" : "false");
	ft_printf("isForce: %s\n", data.isForce ? "true" : "false");
	ft_printf("logIo: %s\n", data.logIo);
	ft_printf("logIn: %s\n", data.logIn);
	ft_printf("logOut: %s\n", data.logOut);
	ft_printf("logTiming: %s\n", data.logTiming);
	ft_printf("loggingFormat: %d\n", data.loggingFormat);
	ft_printf("outputLimit: %u\n", (unsigned int)data.outputLimit);
	ft_printf("isQuiet: %s\n", data.isQuiet ? "true" : "false");
	ft_printf("timing: %s\n", data.timing);
	ft_printf("isClosing: %s\n", data.isClosing ? "true" : "false");
}

int	main(int argc, char **argv)
{
	t_script_settings	data;
	ft_bzero(&data, sizeof(t_script_settings));

	if (parsing(argc, argv, &data))
		return 1;
	print_settings(data);
	if (data.isClosing)
		return 0;

	return 0;
}
