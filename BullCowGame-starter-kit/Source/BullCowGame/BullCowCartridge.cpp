// Fill out your copyright notice in the Description page of Project Settings.
#include "BullCowCartridge.h"
#include "Misc/FileHelper.h"
#include "Misc/Paths.h"

void UBullCowCartridge::BeginPlay() // When the game starts
{
    Super::BeginPlay();
    const FString WordListPath = FPaths::ProjectContentDir() / TEXT("WordsList/WordsList.txt");
    FFileHelper::LoadFileToStringArray(Words, *WordListPath);
    Isograms = GetValidWords(Words);
    SetupGame();    
}

void UBullCowCartridge::OnInput(const FString& Input) // When the player hits enter
{
    if (bGameOver)
    {
        ClearScreen();
        SetupGame();
    }
    else
    {
        /*
        HiddenWord = TEXT("cake"); // move outside this function -> into the header
        PrintLine(TEXT("Hidden word: ") + HiddenWord + ". Entered: " + Input);
        */

        // Check the input
        ProcessGuess(Input);
    }
}

void UBullCowCartridge::ProcessGuess(const FString& Guess)
{
    if (HiddenWord == Guess)
    {
        PrintLine("You guessed correct!");
        // Ask for replay
        EndGame();
    }
    else
    { 
        // Does case matter? Default settings: No
        // Check for right number of letters
        if (HiddenWord.Len() != Guess.Len())
        {
            // Break and Ask for input
            PrintLine(TEXT("The hidden word is %i characters long. Try again!\n"), HiddenWord.Len());
            return;
        }
                
        // Check for isogram
        // Ask to guess again - Don't penalize
        if (!IsIsogram(Guess))
        {
            PrintLine(TEXT("No repearting letters. Guess again.\n"));
            return;
        }
        
        // Depricate remaining lives
        --Lives;
        PrintLine ("Incorrect guess!");
        
        if (Lives == 0) 
        {
            ClearScreen();
            PrintLine(TEXT("No Lives remaining, Game over! The hidden word was %s."), *HiddenWord);
            EndGame(); 
        }
        else 
        {
            bGameOver = false;
            PrintLine(TEXT("You have %i attempts left."), Lives);

            // Check for Bulls and cows
            // Print the Bulls Cows message
            int32 Bulls, Cows;
            GetBullCows(Guess, Bulls, Cows);    
            PrintLine(TEXT("There are %i Bulls and %i Cows. Keep trying!\n"), Bulls, Cows);
        }
    }
}

void UBullCowCartridge::SetupGame()
{       
    HiddenWord = Isograms[FMath::RandRange(0, Isograms.Num() - 1)]; // Set the guess for this instance
    Lives = FMath::Max(8, HiddenWord.Len()*2);
    bGameOver = false;
    
    PrintLine(TEXT("Hi there! Welcome to the Bull Cows game."));
    PrintLine(TEXT("The HiddenWord is %s."), *HiddenWord); // Debug Line

    PrintLine(TEXT("\nGuess a %i letter isogram (No repeated letters)."), HiddenWord.Len()); 
    PrintLine(TEXT("You have %i lives left."), Lives);
    PrintLine(TEXT("Type in and press enter."));
}

bool UBullCowCartridge::IsIsogram(const FString& Word) const
{
    // Compare all characters with each other except for itself.
    // Need to check only the left of the characters.
    for (int8 i = 0; i <= Word.Len() - 2; ++i)
    {
        for (int8 j = i + 1; j <= Word.Len() - 1; ++j)
        {
            if (Word[i] == Word[j]) return false;   
        }
    }
    return true;
}

void UBullCowCartridge::EndGame()
{
    bGameOver = true;
    PrintLine(TEXT("\nPress enter to play again..."));
}

TArray<FString> UBullCowCartridge::GetValidWords(const TArray<FString>& Words) const
{
    TArray<FString> ValidWords;
    for (FString IterWord : Words)
    {
        if (IterWord.Len() > 3 && IterWord.Len() < 9 && IsIsogram(IterWord)) 
            ValidWords.Emplace(IterWord);
    }
    return ValidWords;
}

void UBullCowCartridge::GetBullCows(const FString& Guess, int32& Bulls, int32& Cows) const
{
    Bulls = 0;
    Cows = 0;

    for (int8 Index = 0; Index < Guess.Len(); Index++)
    {
        if (Guess[Index] == HiddenWord[Index])
        {
            Bulls++;
            continue;
        }

        for (int8 j = 0; j < HiddenWord.Len(); j++)
        {
            if (Guess[Index] == HiddenWord[j]) 
            {
                Cows++;
                break;
            }   
        }  
    }
}