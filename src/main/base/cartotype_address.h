/*
cartotype_address.h
Copyright (C) 2013-2022 CartoType Ltd.
See www.cartotype.com for more information.
*/

#pragma once

#include <cartotype_string.h>
#include <cartotype_path.h>
#include <cartotype_feature_info.h>

namespace CartoTypeCore
{

class MapObject;
class StreetAddressSearchStateImplementation;

/** A structured address for use with FindAddress. */
class Address
    {
    public:
    /** Clears an address to its just-constructed state. */
    void Clear();
    /**
    Returns the address as a string.
    If aFull is true supplies the main administrative division
    (state, province, etc.) and country.
    If aLocation is non-null, prefixes that location in degrees to the address.
    */
    String ToString(bool aFull = true,const PointFP* aLocation = nullptr) const;
    /**
    Returns the elements of an address.
    Labels each element with its category (e.g., 'building', 'feature', 'street').
    */
    String ToStringWithLabels() const;
    
    /** The name or number of the building. */
    String Building;
    /** The name of a feature or place of interest. */
    String Feature;
    /** The street, road or other highway. */
    String Street;
    /** The suburb, neighborhood, quarter or other subdivision of the locality. */
    String SubLocality;
    /** The village, town or city. */
    String Locality;
    /** The name of an island. */
    String Island;
    /**
    The subsidiary administrative area: county, district, etc.
    By preference this is a level-6 area in the OpenStreetMap classification.
    Levels 7, 8 and 5 are used in that order if no level-6 area is found.
    */
    String SubAdminArea;
    /**
    The administrative area: state, province, etc.
    By preference this is a level-4 area in the OpenStreetMap classification.
    Level 3 is used if no level-4 area is found.
    */
    String AdminArea;
    /** The country. */
    String Country;
    /** The postal code. */
    String PostCode;
    };

/** A geocode item describes a single map object. */
class GeoCodeItem
    {
    public:
    /** The geocode type of the item. */
    CartoTypeCore::GeoCodeType GeoCodeType = CartoTypeCore::GeoCodeType::None;

    /**
    The name of the object, in the locale used when requesting a geocode.
    For buildings, this may be a building number.
    */
    String Name;
    /** The postal code if any. */
    String PostCode;
    };

}
