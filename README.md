# miscellaneous

## second order system
https://www.youtube.com/watch?v=wrnzFQHhn1I

Just an unreal version implementation from [the video](https://www.youtube.com/watch?v=KPoeNZZ6H4s&t=0s)


# Enhanced JSON Converter for Unreal Engine 5

A simple extension to Unreal's built-in JSON conversion that supports dynamic fields through optional post-processing methods.

```cpp
// Instead of this:
// FJsonObjectConverter::JsonStringToUStruct(JsonString, MyStruct);

// Use this:
FEnhancedJsonConverter::JsonStringToUStruct(JsonString, MyStruct);
```

## Available Functions

```cpp
// JSON string ↔ UStruct
FEnhancedJsonConverter::JsonStringToUStruct(JsonString, OutStruct);
FEnhancedJsonConverter::UStructToJsonString(InStruct, OutJsonString);

// JSON object ↔ UStruct  
FEnhancedJsonConverter::JsonObjectToUStruct(JsonObject, OutStruct);
FEnhancedJsonConverter::UStructToJsonObject(InStruct);
```

```cpp
USTRUCT()
struct FMyData 
{
    GENERATED_BODY()
    
    UPROPERTY()
    FString Name;
    
    TSharedPtr<FJsonObject> DynamicData;  // Dynamic field
    
    // Handle dynamic fields during JSON import
    void PostJsonImport(const TSharedPtr<FJsonObject>& JsonObject)
    {
        if (JsonObject.IsValid())
        {
            const TSharedPtr<FJsonObject>* DynamicPtr;
            if (JsonObject->TryGetObjectField(TEXT("DynamicData"), DynamicPtr))
            {
                DynamicData = *DynamicPtr;
            }
        }
    }
    
    // Handle dynamic fields during JSON export
    TSharedPtr<FJsonObject> PostJsonExport(TSharedPtr<FJsonObject> JsonObject) const
    {
        if (JsonObject.IsValid() && DynamicData.IsValid())
        {
            JsonObject->SetObjectField(TEXT("DynamicData"), DynamicData);
        }
        return JsonObject;
    }
};
```