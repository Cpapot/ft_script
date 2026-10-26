/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parsing.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: coren <coren@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/05 17:23:42 by coco              #+#    #+#             */
/*   Updated: 2026/10/26 11:43:47 by coren            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "parsing.h"

int	find_flag(char *flagsLine)
{
	const char *flagList[] = FLAG_LIST;

	for (int i = 0; i != FLAG_COUNT; i++)
	{
		if (ft_strcmp(flagsLine, flagList[i]) == 0)
			return i + 1;
	}
	return UNKNOWN_F;
}

int	parsing(int argc, char **argv, t_script_data *data)
{
	for (int i = 1; i != argc; i++)
	{
		if (is_flags(argv[i]))
		{
			int flagId = find_flag(argv[i]);
			switch (flagId)
			{
				case -1:
					return 1;
				case UNKNOWN_F:
					return parsing_error(UNKNOWN_FLAG, argv[i], 1);
			}
		}
		else
			return parsing_error(UNKNOWN_PARAM, argv[i], 1);
	}
	(void) data;
	return 0;
}
