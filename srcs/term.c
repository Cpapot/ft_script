/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   term.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: coren <coren@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/29 13:47:41 by coren             #+#    #+#             */
/*   Updated: 2026/10/29 14:21:31 by coren            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <termios.h>
#include "ft_script.h"

int	save_termios_struct(t_script_data *data)
{
	struct termios *term = (struct termios *)data->origTermiosStruct;

	if (tcgetattr(STDIN_FILENO, term) == -1)
	{
		//TODO PRINT ERROR
		return FAILED;
	}
	return SUCCES;
}

int	set_term_to_raw_mode(t_script_data *data)
{
	struct termios rawTerm;
	struct termios *origTerm = (struct termios *)data->origTermiosStruct;

	rawTerm = *origTerm;
	rawTerm.c_lflag &= ~(ECHO | ICANON | ISIG);
	if (tcsetattr(STDIN_FILENO, TCSANOW, &rawTerm) == -1)
	{
		//TODO PRINT ERROR
		return FAILED;
	}
	return SUCCES;
}

int	restore_orig_term(t_script_data *data)
{
	struct termios *origTerm = (struct termios *)data->origTermiosStruct;
	
	if (tcsetattr(STDIN_FILENO, TCSANOW, origTerm) == -1)
	{
		//TODO PRINT ERROR
		return FAILED;
	}
	return SUCCES;
}

