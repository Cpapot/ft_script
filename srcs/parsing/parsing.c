/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parsing.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: coren <coren@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/05 17:23:42 by coco              #+#    #+#             */
/*   Updated: 2026/10/27 13:06:48 by coren            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "parsing.h"

static int	find_flag(char *flagsLine)
{
	const char *flagList[] = FLAG_LIST;
			
	for (int i = 0; i != FLAG_COUNT; i++)
	{
		if (ft_strcmp(flagsLine, flagList[i]))
			return i + 1;
	}
	return UNKNOWN_F;
}

static int find_echo_mode(char *echoMode, t_script_settings *data)
{
	char *echoModeList[] = ECHO_MODE_LIST;

	for (int i = 0; i != ECHO_MODE_COUNT; i++)
	{
		if (ft_strcmp(echoMode, echoModeList[i]))
		{
			data->echoMode = i;
			return SUCCES;
		}
	}
	return parsing_error(UNSSUPORTED_ECHO, echoMode, false, FAILED);
}

static int find_logging_format(char *logingFormat, t_script_settings *data)
{
	char *loggingFormatList[] = LOGGING_FORMAT_LIST;

	for (int i = 0; i != LOGGING_FORMAT_COUNT; i++)
	{
		if (ft_strcmp(logingFormat, loggingFormatList[i]))
		{
			data->loggingFormat = i;
			return SUCCES;
		}
	}
	return parsing_error(UNSSUPORTED_FORMAT, logingFormat, false, FAILED);
}

static int store_data(int flagId, int *index, char **argv, t_script_settings *data)
{
	switch (flagId)
	{
		case UNKNOWN_F:
			return parsing_error(UNKNOWN_FLAG, argv[*index], true, FAILED);
		case APPEND_F:
			data->isAppend = true;
			return SUCCES;
		case COMMAND_F:
			if (argv[*index + 1] == NULL)
				return parsing_error(MISSING_ARG, argv[*index], true, FAILED);
			(*index)++;
			data->command = argv[*index];
			return SUCCES;
		case ECHO_F:
			if (argv[*index + 1] == NULL)
				return parsing_error(MISSING_ARG, argv[*index], true, FAILED);
			(*index)++;
			return find_echo_mode(argv[*index], data);
		case RETURN_F:
			data->isReturn = true;
			return SUCCES;
		case FLUSH_F:
			data->isFlush = true;
			return SUCCES;
		case FORCE_F:
			data->isForce = true;
			return SUCCES;
		case LOG_IO_F:
			if (argv[*index + 1] == NULL)
				return parsing_error(MISSING_ARG, argv[*index], true, FAILED);
			(*index)++;
			data->logIo = argv[*index];
			return SUCCES;
		case LOG_IN_F:
			if (argv[*index + 1] == NULL)
				return parsing_error(MISSING_ARG, argv[*index], true, FAILED);
			(*index)++;
			data->logIn = argv[*index];
			return SUCCES;
		case LOG_OUT_F:
			if (argv[*index + 1] == NULL)
				return parsing_error(MISSING_ARG, argv[*index], true, FAILED);
			(*index)++;
			data->logOut = argv[*index];
			return SUCCES;
		case LOG_TIMING_F:
			if (argv[*index + 1] == NULL)
				return parsing_error(MISSING_ARG, argv[*index], true, FAILED);
			(*index)++;
			data->logTiming = argv[*index];
			return SUCCES;
		case LOGGING_FORMAT_F:
			if (argv[*index + 1] == NULL)
				return parsing_error(MISSING_ARG, argv[*index], true, FAILED);
			(*index)++;
			return find_logging_format(argv[*index], data);
		case OUTPUT_LIMIT_F:
			if (argv[*index + 1] == NULL)
				return parsing_error(MISSING_ARG, argv[*index], true, FAILED);
			(*index)++;
			if (!is_all_numbers(argv[*index]) || !argv[*index][0])
				return parsing_error(FAILED_TO_PARSE_LIMIT, argv[*index], false, FAILED);
			data->outputLimit = ft_atoll(argv[*index]);
			return SUCCES;
		case QUIET_F:
			data->isQuiet = true;
			return SUCCES;
		//cant do -t --timing cause file is optional and should be given in the same string as flag
		//so cause we use strcmp to check flag if we give a file strcmp will be false to fix later
		//or ignore cause --log-timing have the same behavior and the command is deprecated
		case HELP_F:
			write(SCRIPT_WRITE_FD, HELP_FLAG, ft_strlen(HELP_FLAG));
			data->isClosing = true;
			return SUCCES;
		case VERSION_F:
			write(SCRIPT_WRITE_FD, VERSION_FLAG, ft_strlen(VERSION_FLAG));
			data->isClosing = true;
			return SUCCES;
	}
	return FAILED;
}

int	parsing(int argc, char **argv, t_script_settings *data)
{
	if (argc == 0 || argv == NULL || data == NULL)
		return FAILED;
	for (int i = 1; i != argc; i++)
	{
		if (is_flags(argv[i]))
		{
			int flagId = find_flag(argv[i]);
			flagId = flagId / 2 + flagId % 2;
			if (flagId == -1)
				return FAILED;
			if (store_data(flagId, &i, argv, data))
				return FAILED;
			if (data->isClosing)
				return SUCCES;
		}
		else
		{
			if (data->target == NULL)
				data->target = argv[i];
			else
				return parsing_error(UNEXPECTED_ARG, NULL, true, 1);
		}
	}
	if (data->target == NULL)
		data->target = "typescript";
	return SUCCES;
}
