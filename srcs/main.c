/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: coren <coren@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/05 17:06:26 by coco              #+#    #+#             */
/*   Updated: 2026/10/26 13:23:39 by coren            ###   ########.fr       */
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

int	main(int argc, char **argv)
{
	t_script_settings	data;
	ft_bzero(&data, sizeof(t_script_settings));

	if (parsing(argc, argv, &data))
		return 1;

	
	return 0;
}
