#pragma once

class Nonconstructible
{
public:
	Nonconstructible() = delete;
	Nonconstructible(const Nonconstructible&) = delete;
	Nonconstructible operator=(const Nonconstructible&) = delete;
};
