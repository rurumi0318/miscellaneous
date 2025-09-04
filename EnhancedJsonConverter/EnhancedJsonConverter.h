// EnhancedJsonConverter.h
// Enhanced JSON Converter for Unreal Engine 5
// 
// MIT License
// Copyright (c) 2024
// 
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
// 
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.

#pragma once

#include "CoreMinimal.h"
#include "JsonObjectConverter.h"

/**
 * Enhanced JSON Converter for Unreal Engine 5
 * 
 * Problem: Some JSON fields can't be UPROPERTY (e.g., TSharedPtr<FJsonObject>, dynamic data)
 * Solution: Add optional PostJsonImport/PostJsonExport methods to your structs for custom handling
 * 
 * Usage:
 * - Add PostJsonImport/PostJsonExport methods to structs that need dynamic JSON fields
 * - Use FEnhancedJsonConverter::JsonObjectToUStruct and UStructToJsonObject instead of FJsonObjectConverter
 */
namespace FEnhancedJsonConverter
{
    // ========== Type Trait Detectors ==========
    
    /**
     * Detects if a struct has a PostJsonImport method
     * Expected signature: void PostJsonImport(const TSharedPtr<FJsonObject>& JsonObject)
     */
    template<typename T>
    class THasPostJsonImport
    {
        typedef char YesType[1];
        typedef char NoType[2];

        template<typename C> static YesType& Test(decltype(&C::PostJsonImport));
        template<typename C> static NoType& Test(...);

    public:
        enum { Value = sizeof(Test<T>(0)) == sizeof(YesType) };
    };

    /**
     * Detects if a struct has a PostJsonExport method
     * Expected signature: TSharedPtr<FJsonObject> PostJsonExport(TSharedPtr<FJsonObject> JsonObject) const
     */
    template<typename T>
    class THasPostJsonExport
    {
        typedef char YesType[1];
        typedef char NoType[2];

        template<typename C> static YesType& Test(decltype(&C::PostJsonExport));
        template<typename C> static NoType& Test(...);

    public:
        enum { Value = sizeof(Test<T>(0)) == sizeof(YesType) };
    };

    // ========== Internal Helper Functions ==========
    
    namespace Internal
    {
        /**
         * Calls PostJsonImport on the struct if it exists
         */
        template<typename T>
        void CallPostJsonImportIfExists(T& Struct, const TSharedPtr<FJsonObject>& JsonObject)
        {
            if constexpr (THasPostJsonImport<T>::Value)
            {
                Struct.PostJsonImport(JsonObject);
            }
        }

        /**
         * Calls PostJsonExport on the struct if it exists
         */
        template<typename T>
        TSharedPtr<FJsonObject> CallPostJsonExportIfExists(const T& Struct, TSharedPtr<FJsonObject> JsonObject)
        {
            if constexpr (THasPostJsonExport<T>::Value)
            {
                return Struct.PostJsonExport(JsonObject);
            }
            else
            {
                return JsonObject;
            }
        }
    }

    // ========== Public API ==========
    
    /**
     * Converts a JSON object to a UStruct with enhanced support for custom fields
     * 
     * @param JsonObject The JSON object to convert
     * @param OutStruct The struct to populate
     * @return true if conversion was successful
     */
    template<typename TStruct>
    bool JsonObjectToUStruct(const TSharedPtr<FJsonObject>& JsonObject, TStruct& OutStruct)
    {
        if (!JsonObject.IsValid())
        {
            return false;
        }

        // First, use Unreal's standard converter for UPROPERTY fields
        if (!FJsonObjectConverter::JsonObjectToUStruct<TStruct>(JsonObject.ToSharedRef(), &OutStruct))
        {
            return false;
        }

        // Then call PostJsonImport for custom fields (if it exists)
        Internal::CallPostJsonImportIfExists(OutStruct, JsonObject);
        
        return true;
    }

    /**
     * Converts a UStruct to a JSON object with enhanced support for custom fields
     * 
     * @param InStruct The struct to convert
     * @return The JSON object, or nullptr if conversion failed
     */
    template<typename TStruct>
    TSharedPtr<FJsonObject> UStructToJsonObject(const TStruct& InStruct)
    {
        // First, use Unreal's standard converter for UPROPERTY fields
        TSharedPtr<FJsonObject> JsonObject = FJsonObjectConverter::UStructToJsonObject(InStruct);
        
        if (!JsonObject.IsValid())
        {
            return nullptr;
        }

        // Then call PostJsonExport for custom fields (if it exists)
        return Internal::CallPostJsonExportIfExists(InStruct, JsonObject);
    }

    /**
     * Converts a JSON string to a UStruct with enhanced support
     * 
     * @param JsonString The JSON string to parse
     * @param OutStruct The struct to populate
     * @return true if conversion was successful
     */
    template<typename TStruct>
    bool JsonStringToUStruct(const FString& JsonString, TStruct& OutStruct)
    {
        TSharedPtr<FJsonObject> JsonObject;
        TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(JsonString);
        
        if (!FJsonSerializer::Deserialize(Reader, JsonObject) || !JsonObject.IsValid())
        {
            return false;
        }
        
        return JsonObjectToUStruct(JsonObject, OutStruct);
    }

    /**
     * Converts a UStruct to a JSON string with enhanced support
     * 
     * @param InStruct The struct to convert
     * @param OutJsonString The resulting JSON string
     * @return true if conversion was successful
     */
    template<typename TStruct>
    bool UStructToJsonString(const TStruct& InStruct, FString& OutJsonString)
    {
        TSharedPtr<FJsonObject> JsonObject = UStructToJsonObject(InStruct);
        
        if (!JsonObject.IsValid())
        {
            return false;
        }
        
        TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&OutJsonString);
        return FJsonSerializer::Serialize(JsonObject.ToSharedRef(), Writer);
    }
}
