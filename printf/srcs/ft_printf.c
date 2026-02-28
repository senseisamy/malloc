/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_printf.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: snaji <snaji@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/11/09 21:39:03 by snaji             #+#    #+#             */
/*   Updated: 2022/11/14 15:48:10 by snaji            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../headers/utils.h"
#include <stdarg.h>

int	ft_printf(const char *format, ...)
{
	int		count;
	va_list	args;

	va_start(args, format);
	count = ft_vdprintf(1, format, args);
	va_end(args);
	return (count);
}
