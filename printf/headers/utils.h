/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.h                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: snaji <snaji@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/11/10 16:52:24 by snaji             #+#    #+#             */
/*   Updated: 2022/11/14 18:13:21 by snaji            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef UTILS_H
# define UTILS_H
# include <stdarg.h>
# include <string.h>

typedef struct s_flags
{
	int	hashtag;
	int	space;
	int	plus;
}	t_flags;

int	ft_printf_c(char c, int fd);
int	ft_printf_s(char *s, int fd);
int	ft_printf_p(void *addr, int fd);
int	ft_printf_di(int n, int fd, t_flags flags);
int	ft_printf_u(unsigned int n, int fd);
int	ft_printf_x(unsigned int n, int maj, int fd, t_flags flags);
int	ft_vdprintf(int fd, const char *format, va_list ap);
#endif