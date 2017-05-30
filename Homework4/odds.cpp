
void removeOdds(vector<int>& v)
{
	int size = v.size();
	int temp = 0;
	for (int i = 0; i < size; i++)
	{
		if (v[i] % 2 == 1)
		{
			temp = v[i];
			v[i] = v[size - 1];
			v[size] = temp;
			size--;
		}
	}
	v.erase(v.begin() + size, v.end());
}