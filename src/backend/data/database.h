

De database has an index table

struct indexElement {
	int	objectId;
	bool	defined_and_referred;
	std::vector<int> childList;
	std::string theText;
}

std::vector<indexElement> theTable;

void	addElement (x) {
	if (root) => add_root (x);
	if (menu) -> add_menu (x)
	if (list) => add_list (x)
	if (text) => ass_text (x)
	else 	=> ignore
}

void	addMenu (x)
int theId	= getObjectId (x);
int ind		= find (theId);
	if (ind > 0) {	// already referred to
	   if (theTable [ind}. defined_and_referred)
	      return;	// it is there
	   else {
	      for (int i = 0; i < nrChildren; i ++)
	         theTable [ind]. childList. push_back (get_objectType (item (i));
	      theTable [ind]. defined_and_referred = true;
	      for all childre
	}
	else {
	   createNew_element (x);
	   for all children in x do addElement (el);
	}
}

void	createElement (x) {
int index	= firstFreeIndex;
	
	
	


