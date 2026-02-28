/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_printf_s.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: snaji <snaji@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/11/13 15:24:39 by snaji             #+#    #+#             */
/*   Updated: 2022/11/13 16:46:22 by snaji            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <unistd.h>

int	ft_printf_s(char *s, int fd)
{
	int	len;

	if (!s)
		return (ft_printf_s("(null)", fd));
	len = 0;
	while (s[len])
		++len;
	write(fd, s, len);
	return (len);
}
