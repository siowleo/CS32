
// two-parameter overload
void listAll(string path, const Domain* d) // two-parameter overload
{
	const vector<Domain*>& domains = d->subdomains();
	if (!domains.empty())
	{
		for (vector<Domain*>::const_iterator it = domains.begin(); it != domains.end(); it++)
		{
			listAll((*it)->label() + ((path.empty()) ? "" : ".") + path, *it);
		}
	}
	else
	{
		cout << path << endl;
	}
}
