/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_printf_u.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: snaji <snaji@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/11/10 21:14:38 by snaji             #+#    #+#             */
/*   Updated: 2022/11/14 15:57:38 by snaji            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../headers/utils.h"

int	ft_printf_u(unsigned int n, int fd)
{
	if (n < 10)
		return (ft_printf_c(n + 48, fd));
	if (n)
		return (ft_printf_u(n / 10, fd)
			+ ft_printf_c((n % 10) + 48, fd));
	return (0);
}
