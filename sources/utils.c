/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mjamil <mjamil@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/14 10:47:46 by mjamil            #+#    #+#             */
/*   Updated: 2024/12/14 14:53:04 by mjamil           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"

static bool	only_digits(char *str)
{
	int	i;

	i = 0;
	while (str[i])
	{
		if (str[i] < '0' || str[i] > '9')
			return (false);
		i++;
	}
	return (true);
}

int	my_atoi(char *str)
{
	unsigned long long int	nbr;
	int						i;

	i = 0;
	nbr = 0;
	while (str[i] && (str[i] >= '0' && str[i] <= '9'))
	{
		nbr = nbr * 10 + (str[i] - '0');
		i++;
	}
	if (nbr > INT_MAX)
		return (-1);
	return ((int)nbr);
}

bool	is_correct_input(int ac, char **av)
{
	int	i;
	int	nbr;

	i = 1;
	while (i < ac)
	{
		nbr = my_atoi(av[i]);
		if ((!only_digits(av[i])) || (i != 1 && nbr == -1))
		{
			printf("Error : input must be digits only\n");
			return (false);
		}
		if (i == 1 && (nbr <= 0 || nbr > 250))
		{
			printf("there must be between 1 and 250 philosophers.\n");
			return (false);
		}
		i++;
	}
	return (true);
}
