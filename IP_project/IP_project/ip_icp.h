#pragma once

template <typename TYPE>
class IPICP {
public:
	IPICP();
	~IPICP();

	void initial_registration();
private:
	void calculate_coi();
};