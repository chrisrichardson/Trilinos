#ifndef XPETRA_SPLITTINGDRIVER_DECL_HPP
#define XPETRA_SPLITTINGDRIVER_DECL_HPP

#include "Xpetra_Map.hpp"
#include <string>
#include <fstream>
#include <vector>

namespace Xpetra{

template<class GlobalOrdinal>
bool compareRegions(const std::tuple<GlobalOrdinal, GlobalOrdinal> &, const std::tuple<GlobalOrdinal, GlobalOrdinal> &);

template<class GlobalOrdinal>
bool compareNodes(const std::tuple<GlobalOrdinal, GlobalOrdinal> &, const std::tuple<GlobalOrdinal, GlobalOrdinal> &);

//Definition of the predicate for the node_ structure.
//Given a tuple made of node index and a specific region it belongs to,
//this predicate returns true if the node belongs to the region specified in input to the predicate.
template<class GlobalOrdinal>
class checkerNode { 
 
	public:  

		//Constructor
		checkerNode( GlobalOrdinal region_index){region_index_ = region_index;};

		//Unary Operator
  		bool operator()(const std::tuple<GlobalOrdinal, GlobalOrdinal> &node)  
  		{ return (std::get<1>(node) == region_index_); }  

	private:

		GlobalOrdinal region_index_;

};

 
//Definition of the predicate for the nodesToRegion_ sitructure
//Given a tuple made of node index and a vector with labels of regions it belongs to,
//this predicate returns true if the node coincides with the node specified in input to the predicate.
template<class GlobalOrdinal>
class checkerNodesToRegion { 
 
	public:  

		//Constructor
		checkerNodesToRegion( GlobalOrdinal node_index){node_index_ = node_index;};

		//Unary Operator
  		bool operator()(const std::tuple<GlobalOrdinal, Array<GlobalOrdinal> > &node)  
  		{ return (std::get<0>(node) == node_index_); }  

	private:

		GlobalOrdinal node_index_;

}; 


//This is an auxiliary class to store row maps for the global matrix, regional matrices and
//a regionToAll map to link regional node indices with the global ones
template <class Scalar, class LocalOrdinal, class GlobalOrdinal, class Node>
	class Splitting_MapsInfo{
	public:
		Array<Array< std::tuple<GlobalOrdinal,GlobalOrdinal> > > regionToAll_;//used as a map for a RegionToAll node index
		Array<GlobalOrdinal> global_map_; //used as RowMap for global matrices
		Array<Array<GlobalOrdinal> > regional_maps_; //used as RowMap for regional matrices
	};


// This is the actual class that defines the driver
template <class Scalar, class LocalOrdinal, class GlobalOrdinal, class Node>
  	class SplittingDriver{

	public:

	//! @name Constructor/Destructor Methods
	//@{

		//! Constructor specifying the file name containing regional information.
		SplittingDriver (const std::string &, RCP< const Teuchos::Comm<int> >);

	//}
	//! @Interface methods
	//@{
		GlobalOrdinal GetNumGlobalElements(){return num_total_nodes_;};
		GlobalOrdinal GetNumTotalRegions(){return num_total_regions_;};
		Array<GlobalOrdinal> GetGlobalRowMap(){return maps_.global_map_;};
		Array<GlobalOrdinal> GetRegionalRowMap(GlobalOrdinal region_index);
		Array<Array<GlobalOrdinal> > GetRegionalRowMaps(){return maps_.regional_maps_;};
		Array< std::tuple<GlobalOrdinal,GlobalOrdinal> >  GetRegionToAll(GlobalOrdinal);//used as a map for a RegionToAll node index
		Array<std::tuple<int, Array<GlobalOrdinal> > > GetInterfaceNodes(){return interfaceNodes_;};
	//}
	//! @Printout methods
		void printView();
		void printNodesToRegion();
		void printInactive();
	//}

		void CreateRowMaps();

	// Publicly accessible vector which contains the number of regional nodes for each domain region
		Array<GlobalOrdinal> num_regional_nodes_;
		
	private:
		RCP< const Teuchos::Comm<int> > comm_;
		bool nodes_sorted_by_regions_ = false;

		//Global information
		GlobalOrdinal num_total_nodes_ = 0;
		GlobalOrdinal num_total_regions_ = 0;	
		Array<std::tuple<GlobalOrdinal, GlobalOrdinal> > nodes_;//basic structure that imports the information from the input file

		//the following two Array are used to handle the situation where either the number of processes exceeds the number of regions or viceversa
		Array<GlobalOrdinal> regions_per_proc_;//if num_proc > num_regions, then it says how many regions are owned by a single process, empty otherwise
		Array<std::tuple<int, Array<GlobalOrdinal> > > procs_per_region_; //lists of processes instantiated for each region

		Array<std::tuple<int, Array<GlobalOrdinal> > > nodesToRegion_; //for each node it lists the regions it belongs to
		Array<std::tuple<int, Array<GlobalOrdinal> > > interfaceNodes_; //for each node on the interface it lists the regions it belongs to

		//Maps used for global and regional operators
		Splitting_MapsInfo<Scalar, LocalOrdinal, GlobalOrdinal, Node> maps_;

		//Interface routines
		void ReadFileInfo(const std::string &);
		void ComputeProcRegions();
		void NodesToRegion();

  	}; //class SplittingDriver

//This compare class is used to run the sorting algorithm on the list of nodes with associated regions they belong to. 
//First, nodes are sorted in ascending order for region labels. Then, the sorting shuffles the nodes in ascending node index for
//each given region
template<class GlobalOrdinal>
	bool compareRegions(const std::tuple<GlobalOrdinal, GlobalOrdinal> &lhs, const std::tuple<GlobalOrdinal, GlobalOrdinal> &rhs)
	{
		//First we prioritize the sorting according to the region label
		//If the region is the same, then the sorting looks at the global node index
		if( std::get<1>(lhs) < std::get<1>(rhs) )
			return true;
		else if( std::get<1>(lhs) == std::get<1>(rhs) )
			return std::get<0>(lhs) < std::get<0>(rhs); 
		else
			return false;
	}

//This compare is sed to run the sorting algorithm where the nodes are ordered in ascendin order for thei node indes, regardless of the
//associated region index
template<class GlobalOrdinal>
	bool compareNodes(const std::tuple<GlobalOrdinal, GlobalOrdinal> &lhs, const std::tuple<GlobalOrdinal, GlobalOrdinal> &rhs)
	{
		return std::get<0>(lhs) < std::get<0>(rhs); 
	}

} //namespace Xpetra

#endif
