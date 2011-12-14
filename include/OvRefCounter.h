#pragma once

/* �̸��� ���۰����� �ٲܱ�? */

class OvRefable;
class OvRefCounter : public OvMemObject
{
public:
	OvRefCounter( OvRefable *o );
	~OvRefCounter();

	void	inc_strong();
	void	dec_strong();

	void	inc_weak();
	void	dec_weak();

	void	inc();
	void	dec();

	void	dumping();

	OvInt	getscnt() { return scnt; };
	OvInt	getwcnt() { return wcnt; };
	OvRefable* getref() { return ref; };

private:

	OvInt		scnt;
	OvInt		wcnt;
	OvRefable	*ref;
};