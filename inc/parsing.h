/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parsing.h                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: coren <coren@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/05 17:25:27 by coco              #+#    #+#             */
/*   Updated: 2026/10/26 11:39:51 by coren            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef PARSING_H
# define PARSING_H

# include "ft_script.h"

# define FLAG_LIST {"-a" "--append" "-c" "--command" "-E" "--echo" "-e" "--return" \
					"-B" "--log-io" "-I" "--log-in" "-O" "--log-out" "-T" "--log-timing" \
					"-o" "--output-limit" "-q" "--quiet" "-t" "--timing" "-h" "--help" \
					"-V" "--version" "--force"}
# define FLAG_COUNT 27

/*				parser error				*/
# define UNKNOWN_FLAG				"Unknown flag: "
# define UNKNOWN_PARAM				"Unknown parameter: "
# define MISSING_ARG				"Missing argument for flag: "

bool	is_flags(char *str);
bool	is_in_string(char a, char *str);
bool	is_all_numbers(char *str);
int		parsing_error(char *error_type, char *error_info, int ret_value);

enum	e_parsing_flags {
	UNKNOWN_F = 0,
};

enum	e_parsing_return {
	SUCCES = 0,
};


#endif
