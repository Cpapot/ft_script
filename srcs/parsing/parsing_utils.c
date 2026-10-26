/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parsing_utils.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: coren <coren@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/05 17:28:28 by coco              #+#    #+#             */
/*   Updated: 2026/10/26 11:41:21 by coren            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "parsing.h"

/**
 * @brief Determines if a string represents a command-line flag
 * @param str String to check
 * @return true if string starts with '--' and has at least 3 characters, false otherwise
 */
bool	is_flags(char *str)
{
	if (str != NULL)
	{
		if (str[0] == '-' && str[1] == '-' && ft_strlen(str) >= 3)
			return true;
	}
	return false;
}

bool	is_all_numbers(char *str)
{
	for (int i = 0; str[i] != 0; i++)
	{
		if (!ft_isdigit(str[i]))
			return false;
	}
	return true;
}

/**
 * @brief Checks if a character exists within a string
 * @param a Character to search for
 * @param str String to search within
 * @return true if character is found, false otherwise
 */
bool	is_in_string(char a, char *str)
{
	for (int i = 0; str[i]; i++)
	{
		if (str[i] == a)
			return true;
	}
	return false;
}

int		parsing_error(char *error_type, char *error_info, int ret_value)
{
	write(1, ERROR_PRINT, ft_strlen(ERROR_PRINT));
	write(1, error_type, ft_strlen(error_type));
	if (error_info != NULL)
		write(1, error_info, ft_strlen(error_info));
	write(1, "\n", 1);

	return ret_value;
}
