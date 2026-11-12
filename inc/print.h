/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   print.h                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: coren <coren@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/27 15:06:36 by coren             #+#    #+#             */
/*   Updated: 2026/11/12 14:34:43 by coren            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef PRINT_H
# define PRINT_H

# include "ft_script.h"

# define BUFFER_SIZE	1024

# define END_MESSAGE	"Ft_script done."
# define START_MESSAGE	"Ft_script started"
# define OUTPUT_MESSAGE	", output log file is `"
# define INPUT_MESSAGE	", input log file is `"
# define TIMING_MESSAGE	", timing file is `"

# define START_FILE_MESSAGE {"Ft_script started on ", " [TERM=\"", "\" TTY=\"" \
	, "\" COLUMNS=\"", "\" LINES=\"", "\"]\n"}

# define END_FILE_MESSAGE {"\nScript done on ", " [COMMAND_EXIT_CODE=\"", "\"]\n" }


#endif
