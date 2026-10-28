/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: coren <coren@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/05 17:06:26 by coco              #+#    #+#             */
/*   Updated: 2026/10/28 09:32:52 by coren            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_script.h"

int		parsing(int argc, char **argv, t_script_settings *data);

int		create_all_files(t_script_data *data);
void	close_all_files(t_script_data data);
void	print_start_info(t_script_data data);


int	main(int argc, char **argv)
{
	t_script_data	data;
	ft_bzero(&data, sizeof(t_script_data));

	if (parsing(argc, argv, &(data.settings)))
		return FAILED;
	if (data.settings.isClosing)
		return SUCCES;

	print_start_info(data);

	if (create_all_files(&data))
	{
		ft_printf("ok");
		return FAILED;
	}

	close_all_files(data);	
	return SUCCES;
}
