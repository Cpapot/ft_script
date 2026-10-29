/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   files.h                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: coren <coren@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/27 14:01:28 by coren             #+#    #+#             */
/*   Updated: 2026/10/29 12:17:26 by coren            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef FILES_H
# define FILES_H

# include "ft_script.h"
# include <fcntl.h>
# include <unistd.h>
# include <sys/ioctl.h>


# define EACCES_ERROR	"Permission denied cannot open: "
# define EISDIR_ERROR	"Is a directory cannot open: "
# define ENOENT_ERROR	"No such file or directory cannot open: "
# define OPEN_ERROR		"Unknown problem cannot open: "


#endif